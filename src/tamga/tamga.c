/* tamga.c — dil dis cephesi: ayristir + yorumla. */
#include "tamga.h"
#include "ayristirici.h"
#include "../cekirdek/syscall.h"
#include <stdio.h>
#include <string.h>

static bool ifade_dugumu_mu(DugumTuru t) {
    return t == D_SAYI || t == D_METIN || t == D_MANTIK || t == D_BOS ||
           t == D_DEGISKEN || t == D_IKILI || t == D_BIRLI || t == D_CAGRI ||
           t == D_DIZI || t == D_SOZLUK || t == D_INDEKS;
}

bool tamga_calistir_ortamda(Yorumlayici *y, const char *kaynak, bool repl_yaz) {
    Ayristirici a;
    ayristirici_baslat(&a, kaynak);
    Dugum *prog = ayristir(&a);
    if (a.hata || !prog) {
        y->hata = true;
        snprintf(y->mesaj, sizeof y->mesaj, "Sözdizimi hatası: %.130s", a.mesaj);
        return false;
    }

    /* calisma durumunu sifirla, kapsami koru */
    y->hata = false;
    y->donduruyor = false;
    y->dongu_sinyali = SINYAL_YOK;
    y->mesaj[0] = '\0';

    /* REPL: tek ifade satirinin sonucunu otomatik yaz */
    if (repl_yaz && prog->sonraki == NULL && ifade_dugumu_mu(prog->tur)) {
        Deger v = yorumla_ifade(y, &y->kuresel, prog);
        if (!y->hata && v.tur != DEG_BOS) {
            char t[DEGER_METIN_AZAMI];
            deger_yazdir(v, t, sizeof t);
            sys_yaz(t); sys_yaz("\n");
        }
    } else {
        yorumla(y, prog);
    }

    /* Bellek: islev tanimlamayan satirlarin AST'sini serbest birak (arena
       birikmesini onler). Degiskenler/diziler/sozlukler ayri ayrildigindan
       guvenli. İslev tanimi varsa AST korunur (deger dugume atifta bulunur). */
    bool islev_var = false;
    for (Dugum *d = prog; d; d = d->sonraki)
        if (d->tur == D_ISLEV) { islev_var = true; break; }
    if (!islev_var) agac_serbest(prog);

    return !y->hata;
}

bool tamga_calistir(const char *kaynak, char *hata, int boyut) {
    Yorumlayici y;
    yorumlayici_baslat(&y);

    Ayristirici a;
    ayristirici_baslat(&a, kaynak);
    Dugum *prog = ayristir(&a);
    if (a.hata || !prog) {
        if (hata) snprintf(hata, boyut, "Sözdizimi hatası: %.130s", a.mesaj);
        return false;
    }
    yorumla(&y, prog);
    bool ok = !y.hata;
    if (!ok && hata) snprintf(hata, boyut, "Çalışma hatası: %.130s", y.mesaj);
    agac_serbest(prog);   /* taze yorumlayici atilir; AST guvenle serbest */
    return ok;
}
