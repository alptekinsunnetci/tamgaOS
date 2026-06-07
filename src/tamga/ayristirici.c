/* ayristirici.c — ozyineli inis parser.
 * Gramer ozeti icin: docs/tamga-dili.md
 */
#include "ayristirici.h"
#include <string.h>
#include <stdio.h>

/* Tanimlayici/ad kopyalama: her zaman null-sonlandirir, guvenle kirpar. */
static void ad_kopya(char *hedef, const char *kaynak) {
    snprintf(hedef, BELIRTEC_AZAMI_UZUNLUK, "%s", kaynak);
}

/* --- belirtec akisi yardimcilari --- */

static void ilerle(Ayristirici *a) {
    a->simdi = a->ileri;
    a->ileri = tarayici_sonraki(&a->tar);
}

void ayristirici_baslat(Ayristirici *a, const char *kaynak) {
    tarayici_baslat(&a->tar, kaynak);
    a->hata = false;
    a->mesaj[0] = '\0';
    a->hata_satir = 0;
    a->simdi = tarayici_sonraki(&a->tar);
    a->ileri = tarayici_sonraki(&a->tar);
}

static void hata_ver(Ayristirici *a, const char *ne) {
    if (a->hata) return; /* ilk hatayi koru */
    a->hata = true;
    a->hata_satir = a->simdi.satir;
    snprintf(a->mesaj, sizeof a->mesaj, "satir %d: %s (gorulen: %s)",
             a->simdi.satir, ne, belirtec_adi(a->simdi.tur));
}

static bool eslesir(Ayristirici *a, BelirtecTuru tur) {
    if (a->simdi.tur == tur) { ilerle(a); return true; }
    return false;
}

static void bekle(Ayristirici *a, BelirtecTuru tur, const char *ne) {
    if (!eslesir(a, tur)) hata_ver(a, ne);
}

static void satirsonu_atla(Ayristirici *a) {
    while (a->simdi.tur == B_SATIRSONU) ilerle(a);
}

static bool blok_sonu_mu(Ayristirici *a) {
    BelirtecTuru t = a->simdi.tur;
    return t == B_BITIR || t == B_DEGILSE || t == B_DOSYASONU;
}

/* '.'dan sonra uye adi olabilecek belirtec mi? (tanimlayici veya anahtar) */
static bool uye_adi_mi(BelirtecTuru t) {
    return t == B_TANIMLAYICI || (t >= B_YAZ && t <= B_DEGIL);
}

/* --- ileri bildirimler --- */
static Dugum *ifade(Ayristirici *a);
static Dugum *blok(Ayristirici *a);
static Dugum *deyim(Ayristirici *a);

/* --- ifade dilbilgisi (oncelik tirmanmasi) --- */

static Dugum *ikili_yap(BelirtecTuru op, Dugum *sol, Dugum *sag) {
    Dugum *d = agac_dugum(D_IKILI);
    d->op = op; d->sol = sol; d->sag = sag;
    return d;
}

static Dugum *atom(Ayristirici *a) {
    Belirtec b = a->simdi;
    switch (b.tur) {
        case B_SAYI: {
            ilerle(a);
            Dugum *d = agac_dugum(D_SAYI); d->sayi = b.sayi; d->satir = b.satir; return d;
        }
        case B_METIN: {
            ilerle(a);
            Dugum *d = agac_dugum(D_METIN);
            ad_kopya(d->metin, b.metin);
            return d;
        }
        case B_DOGRU: case B_YANLIS: {
            ilerle(a);
            Dugum *d = agac_dugum(D_MANTIK); d->mantik = (b.tur == B_DOGRU); return d;
        }
        case B_BOS:
            ilerle(a);
            return agac_dugum(D_BOS);
        case B_PARANTEZ_AC: {
            ilerle(a);
            Dugum *ic = ifade(a);
            bekle(a, B_PARANTEZ_KAPA, "')' bekleniyordu");
            return ic;
        }
        case B_KOSE_AC: {  /* dizi literali: [a, b, c] veya [] */
            ilerle(a);
            Dugum *d = agac_dugum(D_DIZI);
            Dugum *bas = NULL, *son = NULL;
            if (a->simdi.tur != B_KOSE_KAPA) {
                do {
                    Dugum *oge = ifade(a);
                    if (a->hata) return NULL;
                    if (!bas) bas = son = oge; else { son->sonraki = oge; son = oge; }
                } while (eslesir(a, B_VIRGUL));
            }
            bekle(a, B_KOSE_KAPA, "']' bekleniyordu");
            d->argumanlar = bas;
            return d;
        }
        case B_SUSLU_AC: {  /* sözlük literali: { "k": v, ... } veya {} */
            ilerle(a);
            Dugum *d = agac_dugum(D_SOZLUK);
            Dugum *bas = NULL, *son = NULL;
            if (a->simdi.tur != B_SUSLU_KAPA) {
                do {
                    Dugum *anahtar = ifade(a);
                    bekle(a, B_IKINOKTA, "':' bekleniyordu");
                    Dugum *deger = ifade(a);
                    if (a->hata) return NULL;
                    /* anahtar ve deger sirayla zincire eklenir */
                    if (!bas) bas = son = anahtar; else { son->sonraki = anahtar; son = anahtar; }
                    son->sonraki = deger; son = deger;
                } while (eslesir(a, B_VIRGUL));
            }
            bekle(a, B_SUSLU_KAPA, "'}' bekleniyordu");
            d->argumanlar = bas;
            return d;
        }
        case B_TANIMLAYICI: {
            char ad[BELIRTEC_AZAMI_UZUNLUK];
            char modul[BELIRTEC_AZAMI_UZUNLUK] = "";
            ad_kopya(ad, b.metin);
            ilerle(a);
            /* modul.uye? */
            if (a->simdi.tur == B_NOKTA) {
                ilerle(a);
                if (!uye_adi_mi(a->simdi.tur)) { hata_ver(a, "'.' sonrasi uye adi bekleniyordu"); return NULL; }
                ad_kopya(modul, ad);
                ad_kopya(ad, a->simdi.metin);
                ilerle(a);
            }
            /* cagri? */
            if (a->simdi.tur == B_PARANTEZ_AC) {
                ilerle(a);
                Dugum *c = agac_dugum(D_CAGRI);
                ad_kopya(c->metin, ad);
                ad_kopya(c->modul, modul);
                /* argumanlar */
                Dugum *bas = NULL, *son = NULL;
                if (a->simdi.tur != B_PARANTEZ_KAPA) {
                    do {
                        Dugum *arg = ifade(a);
                        if (a->hata) return NULL;
                        if (!bas) bas = son = arg; else { son->sonraki = arg; son = arg; }
                    } while (eslesir(a, B_VIRGUL));
                }
                bekle(a, B_PARANTEZ_KAPA, "')' bekleniyordu");
                c->argumanlar = bas;
                return c;
            }
            /* duz degisken */
            Dugum *d = agac_dugum(D_DEGISKEN);
            ad_kopya(d->metin, ad);
            return d;
        }
        default:
            hata_ver(a, "ifade bekleniyordu");
            return NULL;
    }
}

/* atom + sonek indeksleme: e[i], e[i][j] ... */
static Dugum *birincil(Ayristirici *a) {
    Dugum *e = atom(a);
    while (a->simdi.tur == B_KOSE_AC && !a->hata) {
        ilerle(a);
        Dugum *idx = ifade(a);
        bekle(a, B_KOSE_KAPA, "']' bekleniyordu");
        Dugum *d = agac_dugum(D_INDEKS);
        d->sol = e;
        d->sag = idx;
        e = d;
    }
    return e;
}

static Dugum *birli(Ayristirici *a) {
    if (a->simdi.tur == B_EKSI || a->simdi.tur == B_DEGIL) {
        BelirtecTuru op = a->simdi.tur;
        ilerle(a);
        Dugum *sag = birli(a);
        Dugum *d = agac_dugum(D_BIRLI);
        d->op = op; d->sag = sag;
        return d;
    }
    return birincil(a);
}

static Dugum *carpma(Ayristirici *a) {
    Dugum *sol = birli(a);
    while (a->simdi.tur == B_CARPI || a->simdi.tur == B_BOLU || a->simdi.tur == B_MOD) {
        BelirtecTuru op = a->simdi.tur; ilerle(a);
        sol = ikili_yap(op, sol, birli(a));
    }
    return sol;
}

static Dugum *toplama(Ayristirici *a) {
    Dugum *sol = carpma(a);
    while (a->simdi.tur == B_ARTI || a->simdi.tur == B_EKSI) {
        BelirtecTuru op = a->simdi.tur; ilerle(a);
        sol = ikili_yap(op, sol, carpma(a));
    }
    return sol;
}

static Dugum *karsilastirma(Ayristirici *a) {
    Dugum *sol = toplama(a);
    while (a->simdi.tur == B_ESIT || a->simdi.tur == B_ESITDEGIL ||
           a->simdi.tur == B_KUCUK || a->simdi.tur == B_BUYUK ||
           a->simdi.tur == B_KUCUKESIT || a->simdi.tur == B_BUYUKESIT) {
        BelirtecTuru op = a->simdi.tur; ilerle(a);
        sol = ikili_yap(op, sol, toplama(a));
    }
    return sol;
}

static Dugum *mantik(Ayristirici *a) {
    Dugum *sol = karsilastirma(a);
    while (a->simdi.tur == B_VE || a->simdi.tur == B_VEYA) {
        BelirtecTuru op = a->simdi.tur; ilerle(a);
        sol = ikili_yap(op, sol, karsilastirma(a));
    }
    return sol;
}

static Dugum *ifade(Ayristirici *a) { return mantik(a); }

/* --- deyimler --- */

static Dugum *deyim_yaz(Ayristirici *a) {
    ilerle(a); /* 'yaz' */
    Dugum *d = agac_dugum(D_YAZ);
    d->sol = ifade(a);
    return d;
}

static Dugum *deyim_eger(Ayristirici *a) {
    ilerle(a); /* 'eğer' */
    Dugum *d = agac_dugum(D_EGER);
    d->sol = ifade(a);
    bekle(a, B_ISE, "'ise' bekleniyordu");
    d->govde = blok(a);
    if (a->simdi.tur == B_DEGILSE) {
        ilerle(a);
        d->govde2 = blok(a);
    }
    bekle(a, B_BITIR, "'bitir' bekleniyordu");
    return d;
}

static Dugum *deyim_iken(Ayristirici *a) {
    ilerle(a); /* 'iken' */
    Dugum *d = agac_dugum(D_IKEN);
    d->sol = ifade(a);
    d->govde = blok(a);
    bekle(a, B_BITIR, "'bitir' bekleniyordu");
    return d;
}

static Dugum *deyim_her(Ayristirici *a) {
    ilerle(a); /* 'her' */
    Dugum *d = agac_dugum(D_HER);
    if (a->simdi.tur != B_TANIMLAYICI) { hata_ver(a, "döngü değişkeni bekleniyordu"); return d; }
    ad_kopya(d->metin, a->simdi.metin);
    ilerle(a);
    bekle(a, B_ICINDE, "'içinde' bekleniyordu");
    d->sol = ifade(a);              /* koleksiyon */
    d->govde = blok(a);
    bekle(a, B_BITIR, "'bitir' bekleniyordu");
    return d;
}

static Dugum *deyim_islev(Ayristirici *a) {
    ilerle(a); /* 'işlev' */
    Dugum *d = agac_dugum(D_ISLEV);
    if (a->simdi.tur != B_TANIMLAYICI) { hata_ver(a, "islev adi bekleniyordu"); return d; }
    ad_kopya(d->metin, a->simdi.metin);
    ilerle(a);
    bekle(a, B_PARANTEZ_AC, "'(' bekleniyordu");
    Dugum *bas = NULL, *son = NULL;
    if (a->simdi.tur != B_PARANTEZ_KAPA) {
        do {
            if (a->simdi.tur != B_TANIMLAYICI) { hata_ver(a, "parametre adi bekleniyordu"); break; }
            Dugum *p = agac_dugum(D_DEGISKEN);
            ad_kopya(p->metin, a->simdi.metin);
            ilerle(a);
            if (!bas) bas = son = p; else { son->sonraki = p; son = p; }
        } while (eslesir(a, B_VIRGUL));
    }
    bekle(a, B_PARANTEZ_KAPA, "')' bekleniyordu");
    d->parametreler = bas;
    d->govde = blok(a);
    bekle(a, B_BITIR, "'bitir' bekleniyordu");
    return d;
}

static Dugum *deyim_dondur(Ayristirici *a) {
    ilerle(a); /* 'döndür' */
    Dugum *d = agac_dugum(D_DONDUR);
    if (a->simdi.tur != B_SATIRSONU && !blok_sonu_mu(a))
        d->sol = ifade(a);
    return d;
}

static Dugum *deyim(Ayristirici *a) {
    switch (a->simdi.tur) {
        case B_YAZ:    return deyim_yaz(a);
        case B_EGER:   return deyim_eger(a);
        case B_IKEN:   return deyim_iken(a);
        case B_HER:    return deyim_her(a);
        case B_ISLEV:  return deyim_islev(a);
        case B_DONDUR: return deyim_dondur(a);
        case B_DUR:    ilerle(a); return agac_dugum(D_DUR);
        case B_DEVAM:  ilerle(a); return agac_dugum(D_DEVAM);
        default: {
            /* ifade deyimi veya atama. Once lvalue ifadesini ayristir, '=' varsa
               atama yap (hedef: degisken veya dizi ogesi). */
            Dugum *e = ifade(a);
            if (a->simdi.tur == B_ATAMA) {
                if (!e || (e->tur != D_DEGISKEN && e->tur != D_INDEKS)) {
                    hata_ver(a, "atama hedefi degisken veya dizi ogesi olmali");
                    return e;
                }
                ilerle(a); /* '=' */
                Dugum *d = agac_dugum(D_ATAMA);
                d->sol = e;
                d->sag = ifade(a);
                return d;
            }
            return e; /* ifade deyimi; degeri yorumlayicida atilir */
        }
    }
}

static Dugum *blok(Ayristirici *a) {
    Dugum *bas = NULL, *son = NULL;
    satirsonu_atla(a);
    while (!blok_sonu_mu(a) && !a->hata) {
        Dugum *d = deyim(a);
        if (a->hata) break;
        if (d) {
            if (!bas) bas = son = d; else { son->sonraki = d; son = d; }
        }
        satirsonu_atla(a);
    }
    return bas;
}

Dugum *ayristir(Ayristirici *a) {
    Dugum *program = blok(a);
    if (!a->hata && a->simdi.tur != B_DOSYASONU)
        hata_ver(a, "beklenmeyen belirtec (dosya sonu bekleniyordu)");
    if (a->hata) { agac_serbest(program); return NULL; }
    return program;
}
