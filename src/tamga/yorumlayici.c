/* yorumlayici.c — agac-yuruyen yorumlayici.
 *
 * Akis: kuresel islevler on-kayit edilir, sonra deyimler sirayla calistirilir.
 * 'döndür' y->donduruyor bayragiyla yukari tasinir.
 */
#include "yorumlayici.h"
#include "dizi.h"
#include "sozluk.h"
#include "metin_islev.h"
#include "moduller/moduller.h"
#include "../cekirdek/syscall.h"
#include "../cekirdek/bellek.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ARG_AZAMI 8

static void hata(Yorumlayici *y, const char *ne) {
    if (y->hata) return;
    y->hata = true;
    snprintf(y->mesaj, sizeof y->mesaj, "%s", ne);
}

static void blogu_calistir(Yorumlayici *y, Ortam *o, Dugum *deyimler);
static void deyimi_calistir(Yorumlayici *y, Ortam *o, Dugum *d);

void yorumlayici_baslat(Yorumlayici *y) {
    ortam_baslat(&y->kuresel, NULL);
    y->donduruyor = false;
    y->dongu_sinyali = SINYAL_YOK;
    y->donen = deger_bos();
    y->hata = false;
    y->mesaj[0] = '\0';
}

/* --- islev cagrisi --- */
static Deger islev_cagir(Yorumlayici *y, Dugum *islev, Deger *arg, int n) {
    /* Ortam yigin yerine heap'ten (gomulu kucuk yiginlarda ozyineli tasmayi onler) */
    Ortam *yerel = (Ortam *)bellek_ayir(sizeof(Ortam));
    if (!yerel) { hata(y, "bellek yetersiz (işlev çağrısı)"); return deger_bos(); }
    ortam_baslat(yerel, &y->kuresel);
    Dugum *p = islev->parametreler;
    int i = 0;
    while (p) {
        ortam_tanimla(yerel, p->metin, (i < n) ? arg[i] : deger_bos());
        p = p->sonraki; i++;
    }
    bool eski = y->donduruyor;
    int  eski_sinyal = y->dongu_sinyali;  /* dur/devam islev sinirini asmaz */
    y->donduruyor = false;
    y->dongu_sinyali = SINYAL_YOK;
    blogu_calistir(y, yerel, islev->govde);
    Deger sonuc = y->donduruyor ? y->donen : deger_bos();
    y->donduruyor = eski;
    y->dongu_sinyali = eski_sinyal;
    bellek_birak(yerel);
    return sonuc;
}

/* --- ikili islemler --- */
static Deger ikili_uygula(Yorumlayici *y, BelirtecTuru op, Deger s, Deger g) {
    /* '+' metin birlestirme destegi */
    if (op == B_ARTI && (s.tur == DEG_METIN || g.tur == DEG_METIN)) {
        char a[DEGER_METIN_AZAMI], b[DEGER_METIN_AZAMI], c[DEGER_METIN_AZAMI * 2];
        deger_yazdir(s, a, sizeof a);
        deger_yazdir(g, b, sizeof b);
        snprintf(c, sizeof c, "%s%s", a, b);  /* deger_metin 127 bayta kirpar */
        return deger_metin(c);
    }

    switch (op) {
        case B_ARTI:  return deger_sayi(s.sayi + g.sayi);
        case B_EKSI:  return deger_sayi(s.sayi - g.sayi);
        case B_CARPI: return deger_sayi(s.sayi * g.sayi);
        case B_BOLU:
            if (g.sayi == 0) { hata(y, "sifira bolme"); return deger_bos(); }
            return deger_sayi(s.sayi / g.sayi);
        case B_MOD:
            if (g.sayi == 0) { hata(y, "sifira bolme (mod)"); return deger_bos(); }
            return deger_sayi(fmod(s.sayi, g.sayi));
        case B_KUCUK:      return deger_mantik(s.sayi <  g.sayi);
        case B_BUYUK:      return deger_mantik(s.sayi >  g.sayi);
        case B_KUCUKESIT:  return deger_mantik(s.sayi <= g.sayi);
        case B_BUYUKESIT:  return deger_mantik(s.sayi >= g.sayi);
        case B_ESIT:
        case B_ESITDEGIL: {
            bool esit;
            if (s.tur != g.tur) esit = false;
            else if (s.tur == DEG_SAYI)   esit = (s.sayi == g.sayi);
            else if (s.tur == DEG_MANTIK) esit = (s.mantik == g.mantik);
            else if (s.tur == DEG_METIN)  esit = (strcmp(s.metin, g.metin) == 0);
            else if (s.tur == DEG_BOS)    esit = true;
            else                          esit = (s.islev == g.islev);
            return deger_mantik(op == B_ESIT ? esit : !esit);
        }
        default:
            hata(y, "bilinmeyen ikili islem");
            return deger_bos();
    }
}

/* --- yerlesik (global) fonksiyonlar: dizi/metin yardimcilari --- */
static bool yerlesik_cagir(Yorumlayici *y, const char *ad, Deger *arg, int n, Deger *cikti) {
    if (strcmp(ad, "dizi") == 0) {
        *cikti = deger_dizi(dizi_olustur());
        return true;
    }
    if (strcmp(ad, "uzunluk") == 0) {
        if (n < 1) { *cikti = deger_sayi(0); return true; }
        if (arg[0].tur == DEG_DIZI)   { *cikti = deger_sayi(dizi_uzunluk((Dizi *)arg[0].dizi)); return true; }
        if (arg[0].tur == DEG_SOZLUK) { *cikti = deger_sayi(sozluk_uzunluk((Sozluk *)arg[0].sozluk)); return true; }
        if (arg[0].tur == DEG_METIN)  { *cikti = deger_sayi((double)strlen(arg[0].metin)); return true; }
        *cikti = deger_sayi(0);
        return true;
    }
    if (strcmp(ad, "sözlük") == 0 || strcmp(ad, "sozluk") == 0) {
        *cikti = deger_sozluk(sozluk_olustur());
        return true;
    }
    if (strcmp(ad, "anahtarlar") == 0) {
        if (n < 1 || arg[0].tur != DEG_SOZLUK) { hata(y, "anahtarlar(sözlük) bekleniyordu"); return true; }
        Sozluk *sz = (Sozluk *)arg[0].sozluk;
        Dizi *dz = dizi_olustur();
        int m = sozluk_uzunluk(sz);
        for (int i = 0; i < m; i++) dizi_ekle(dz, deger_metin(sozluk_anahtar(sz, i)));
        *cikti = deger_dizi(dz);
        return true;
    }
    if (strcmp(ad, "var_mı") == 0 || strcmp(ad, "var_mi") == 0) {
        if (n < 2 || arg[0].tur != DEG_SOZLUK || arg[1].tur != DEG_METIN) { hata(y, "var_mı(sözlük, metin) bekleniyordu"); return true; }
        *cikti = deger_mantik(sozluk_var((Sozluk *)arg[0].sozluk, arg[1].metin));
        return true;
    }
    if (strcmp(ad, "sil_anahtar") == 0) {
        if (n < 2 || arg[0].tur != DEG_SOZLUK || arg[1].tur != DEG_METIN) { hata(y, "sil_anahtar(sözlük, metin) bekleniyordu"); return true; }
        *cikti = deger_mantik(sozluk_sil((Sozluk *)arg[0].sozluk, arg[1].metin));
        return true;
    }
    if (strcmp(ad, "ekle") == 0) {
        if (n < 2 || arg[0].tur != DEG_DIZI) { hata(y, "ekle(dizi, deger) bekleniyordu"); return true; }
        Dizi *dz = (Dizi *)arg[0].dizi;
        dizi_ekle(dz, arg[1]);
        *cikti = deger_sayi(dizi_uzunluk(dz));
        return true;
    }
    if (strcmp(ad, "cikar") == 0 || strcmp(ad, "çıkar") == 0) {
        if (n < 1 || arg[0].tur != DEG_DIZI) { hata(y, "çıkar(dizi) bekleniyordu"); return true; }
        *cikti = dizi_cikar((Dizi *)arg[0].dizi);
        return true;
    }
    if (strcmp(ad, "oku") == 0) {
        char tampon[DEGER_METIN_AZAMI];
        int u = sys_oku_satir(tampon, sizeof tampon);
        *cikti = (u < 0) ? deger_bos() : deger_metin(tampon);
        return true;
    }
    if (strcmp(ad, "sayı") == 0 || strcmp(ad, "sayi") == 0) {
        if (n < 1) { *cikti = deger_sayi(0); return true; }
        if (arg[0].tur == DEG_SAYI)  { *cikti = arg[0]; return true; }
        if (arg[0].tur == DEG_METIN) { *cikti = deger_sayi(strtod(arg[0].metin, NULL)); return true; }
        if (arg[0].tur == DEG_MANTIK){ *cikti = deger_sayi(arg[0].mantik ? 1 : 0); return true; }
        *cikti = deger_sayi(0);
        return true;
    }
    if (strcmp(ad, "metin") == 0) {
        if (n < 1) { *cikti = deger_metin(""); return true; }
        char tampon[DEGER_METIN_AZAMI];
        deger_yazdir(arg[0], tampon, sizeof tampon);
        *cikti = deger_metin(tampon);
        return true;
    }
    /* metin yerlesikleri (parça/bul/içerir/.../parçala/birleştir) */
    if (metin_yerlesik(ad, arg, n, cikti)) return true;
    return false; /* yerlesik degil */
}

/* --- ifade degerlendirme --- */
Deger yorumla_ifade(Yorumlayici *y, Ortam *o, Dugum *d) {
    if (!d || y->hata) return deger_bos();
    switch (d->tur) {
        case D_SAYI:   return deger_sayi(d->sayi);
        case D_METIN:  return deger_metin(d->metin);
        case D_MANTIK: return deger_mantik(d->mantik != 0);
        case D_BOS:    return deger_bos();

        case D_DEGISKEN: {
            Deger v;
            if (!ortam_al(o, d->metin, &v)) {
                char m[128]; snprintf(m, sizeof m, "tanimsiz degisken: %s", d->metin);
                hata(y, m); return deger_bos();
            }
            return v;
        }

        case D_DIZI: {
            Dizi *dz = dizi_olustur();
            for (Dugum *e = d->argumanlar; e && !y->hata; e = e->sonraki)
                dizi_ekle(dz, yorumla_ifade(y, o, e));
            return deger_dizi(dz);
        }

        case D_SOZLUK: {
            Sozluk *sz = sozluk_olustur();
            /* argumanlar: anahtar, deger, anahtar, deger, ... */
            for (Dugum *e = d->argumanlar; e && e->sonraki && !y->hata; e = e->sonraki->sonraki) {
                Deger ak = yorumla_ifade(y, o, e);
                Deger dg = yorumla_ifade(y, o, e->sonraki);
                if (y->hata) break;
                if (ak.tur != DEG_METIN) { hata(y, "sözlük anahtarı metin olmalı"); break; }
                sozluk_ata(sz, ak.metin, dg);
            }
            return deger_sozluk(sz);
        }

        case D_INDEKS: {
            Deger nesne = yorumla_ifade(y, o, d->sol);
            Deger idx = yorumla_ifade(y, o, d->sag);
            if (y->hata) return deger_bos();
            if (nesne.tur == DEG_SOZLUK) {
                if (idx.tur != DEG_METIN) { hata(y, "sözlük anahtarı metin olmalı"); return deger_bos(); }
                return sozluk_al((Sozluk *)nesne.sozluk, idx.metin);
            }
            if (idx.tur != DEG_SAYI) { hata(y, "indeks sayı olmalı"); return deger_bos(); }
            int i = (int)idx.sayi;
            if (nesne.tur == DEG_DIZI) return dizi_al((Dizi *)nesne.dizi, i);
            if (nesne.tur == DEG_METIN) {
                int u = (int)strlen(nesne.metin);
                if (i < 0 || i >= u) return deger_bos();
                char tek[2] = { nesne.metin[i], '\0' };
                return deger_metin(tek);
            }
            hata(y, "indekslenebilir tür değil (dizi/sözlük/metin bekleniyordu)");
            return deger_bos();
        }

        case D_BIRLI: {
            Deger sag = yorumla_ifade(y, o, d->sag);
            if (d->op == B_EKSI)  return deger_sayi(-sag.sayi);
            if (d->op == B_DEGIL) return deger_mantik(!deger_dogru_mu(sag));
            hata(y, "bilinmeyen birli islem");
            return deger_bos();
        }

        case D_IKILI: {
            /* mantiksal kisa devre */
            if (d->op == B_VE) {
                Deger s = yorumla_ifade(y, o, d->sol);
                if (!deger_dogru_mu(s)) return deger_mantik(false);
                return deger_mantik(deger_dogru_mu(yorumla_ifade(y, o, d->sag)));
            }
            if (d->op == B_VEYA) {
                Deger s = yorumla_ifade(y, o, d->sol);
                if (deger_dogru_mu(s)) return deger_mantik(true);
                return deger_mantik(deger_dogru_mu(yorumla_ifade(y, o, d->sag)));
            }
            Deger s = yorumla_ifade(y, o, d->sol);
            Deger g = yorumla_ifade(y, o, d->sag);
            if (y->hata) return deger_bos();
            return ikili_uygula(y, d->op, s, g);
        }

        case D_CAGRI: {
            Deger arg[ARG_AZAMI];
            int n = 0;
            for (Dugum *a = d->argumanlar; a && n < ARG_AZAMI; a = a->sonraki)
                arg[n++] = yorumla_ifade(y, o, a);
            if (y->hata) return deger_bos();

            if (d->modul[0] != '\0') {
                Deger cikti = deger_bos();
                if (modul_cagir(d->modul, d->metin, arg, n, &cikti)) return cikti;
                char m[128]; snprintf(m, sizeof m, "bilinmeyen modul cagrisi: %.40s.%.40s", d->modul, d->metin);
                hata(y, m); return deger_bos();
            }
            /* kullanici islevi */
            Deger islev;
            if (ortam_al(o, d->metin, &islev) && islev.tur == DEG_ISLEV)
                return islev_cagir(y, (Dugum *)islev.islev, arg, n);
            /* yerlesik fonksiyon (dizi/uzunluk/ekle/cikar) */
            Deger ycikti = deger_bos();
            if (yerlesik_cagir(y, d->metin, arg, n, &ycikti)) return ycikti;
            char m[128]; snprintf(m, sizeof m, "tanimsiz islev: %s", d->metin);
            hata(y, m); return deger_bos();
        }

        default:
            hata(y, "ifade olarak degerlendirilemeyen dugum");
            return deger_bos();
    }
}

/* --- deyim calistirma --- */
static void deyimi_calistir(Yorumlayici *y, Ortam *o, Dugum *d) {
    if (!d || y->hata || y->donduruyor || y->dongu_sinyali) return;
    switch (d->tur) {
        case D_DUR:   y->dongu_sinyali = SINYAL_DUR;   break;
        case D_DEVAM: y->dongu_sinyali = SINYAL_DEVAM; break;
        case D_ATAMA: {
            Deger v = yorumla_ifade(y, o, d->sag);
            if (y->hata) break;
            Dugum *hedef = d->sol;
            if (hedef->tur == D_DEGISKEN) {
                ortam_guncelle(o, hedef->metin, v);
            } else if (hedef->tur == D_INDEKS) {
                Deger nesne = yorumla_ifade(y, o, hedef->sol);
                Deger idx = yorumla_ifade(y, o, hedef->sag);
                if (y->hata) break;
                if (nesne.tur == DEG_SOZLUK) {
                    if (idx.tur != DEG_METIN) { hata(y, "sözlük anahtarı metin olmalı"); break; }
                    sozluk_ata((Sozluk *)nesne.sozluk, idx.metin, v);
                } else if (nesne.tur == DEG_DIZI) {
                    if (idx.tur != DEG_SAYI) { hata(y, "indeks sayı olmalı"); break; }
                    if (!dizi_ata((Dizi *)nesne.dizi, (int)idx.sayi, v)) { hata(y, "indeks aralık dışı"); break; }
                } else {
                    hata(y, "yalnız dizi/sözlük ögesine atanabilir"); break;
                }
            }
            break;
        }
        case D_YAZ: {
            Deger v = yorumla_ifade(y, o, d->sol);
            if (y->hata) break;   /* ifade hatasinda yazma */
            char tampon[DEGER_METIN_AZAMI];
            deger_yazdir(v, tampon, sizeof tampon);
            sys_yaz(tampon);
            sys_yaz("\n");
            break;
        }
        case D_EGER: {
            Deger k = yorumla_ifade(y, o, d->sol);
            if (deger_dogru_mu(k)) blogu_calistir(y, o, d->govde);
            else if (d->govde2)    blogu_calistir(y, o, d->govde2);
            break;
        }
        case D_IKEN: {
            int guvenlik = 0;
            while (!y->hata && !y->donduruyor) {
                Deger k = yorumla_ifade(y, o, d->sol);
                if (!deger_dogru_mu(k)) break;
                blogu_calistir(y, o, d->govde);
                if (y->dongu_sinyali == SINYAL_DUR)   { y->dongu_sinyali = SINYAL_YOK; break; }
                if (y->dongu_sinyali == SINYAL_DEVAM) { y->dongu_sinyali = SINYAL_YOK; }
                if (++guvenlik > 1000000) { hata(y, "dongu sinirini asti"); break; }
            }
            break;
        }
        case D_HER: {
            Deger kol = yorumla_ifade(y, o, d->sol);
            if (y->hata) break;
            if (kol.tur == DEG_DIZI) {
                Dizi *dz = (Dizi *)kol.dizi;
                int m = dizi_uzunluk(dz);
                for (int i = 0; i < m && !y->hata && !y->donduruyor; i++) {
                    ortam_guncelle(o, d->metin, dizi_al(dz, i));
                    blogu_calistir(y, o, d->govde);
                    if (y->dongu_sinyali == SINYAL_DUR)   { y->dongu_sinyali = SINYAL_YOK; break; }
                    if (y->dongu_sinyali == SINYAL_DEVAM) { y->dongu_sinyali = SINYAL_YOK; }
                }
            } else if (kol.tur == DEG_SOZLUK) {
                Sozluk *sz = (Sozluk *)kol.sozluk;
                int m = sozluk_uzunluk(sz);
                for (int i = 0; i < m && !y->hata && !y->donduruyor; i++) {
                    ortam_guncelle(o, d->metin, deger_metin(sozluk_anahtar(sz, i)));
                    blogu_calistir(y, o, d->govde);
                    if (y->dongu_sinyali == SINYAL_DUR)   { y->dongu_sinyali = SINYAL_YOK; break; }
                    if (y->dongu_sinyali == SINYAL_DEVAM) { y->dongu_sinyali = SINYAL_YOK; }
                }
            } else {
                hata(y, "'her' yalnız dizi veya sözlük üzerinde gezer");
            }
            break;
        }
        case D_ISLEV:
            ortam_tanimla(o, d->metin, deger_islev(d));
            break;
        case D_DONDUR:
            y->donen = d->sol ? yorumla_ifade(y, o, d->sol) : deger_bos();
            y->donduruyor = true;
            break;
        default:
            /* ifade deyimi (orn. modul cagrisi); degeri at */
            yorumla_ifade(y, o, d);
            break;
    }
}

static void blogu_calistir(Yorumlayici *y, Ortam *o, Dugum *deyimler) {
    for (Dugum *d = deyimler; d && !y->hata && !y->donduruyor && !y->dongu_sinyali; d = d->sonraki)
        deyimi_calistir(y, o, d);
}

bool yorumla(Yorumlayici *y, Dugum *program) {
    /* 1. gecis: kuresel islevleri on-kayit et (ileri bildirim destegi) */
    for (Dugum *d = program; d; d = d->sonraki)
        if (d->tur == D_ISLEV)
            ortam_tanimla(&y->kuresel, d->metin, deger_islev(d));

    /* 2. gecis: deyimleri calistir (islev tanimlarini atla) */
    for (Dugum *d = program; d && !y->hata; d = d->sonraki) {
        if (d->tur == D_ISLEV) continue;
        deyimi_calistir(y, &y->kuresel, d);
        if (y->donduruyor) break; /* ust seviyede döndür -> dur */
    }
    return !y->hata;
}
