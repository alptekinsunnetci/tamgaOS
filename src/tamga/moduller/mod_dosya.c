/* mod_dosya.c — 'dosya' modulu (TDS sarmalayici).
 * dosya.yaz(yol, icerik)   -> bayt sayisi
 * dosya.ekle(yol, icerik)  -> bayt sayisi
 * dosya.oku(yol)           -> metin icerik
 * dosya.sil(yol)           -> doğru/yanlış
 * dosya.var(yol)           -> doğru/yanlış
 * dosya.listele(dizin)     -> girdi sayisi (ve konsola listeler)
 * Bas '/' yoksa yol /belgeler altina yerlestirilir.
 */
#include "moduller.h"
#include "../../tds/tds.h"
#include "../../cekirdek/syscall.h"
#include <string.h>
#include <stdio.h>

static const char *yolu_coz(const char *girdi, char *tampon, int boyut) {
    if (girdi && girdi[0] == '/') return girdi;
    snprintf(tampon, boyut, "/belgeler/%.48s", girdi ? girdi : "");
    return tampon;
}

static const char *metin_arg(Deger *arg, int n, int i) {
    return (i < n && arg[i].tur == DEG_METIN) ? arg[i].metin : "";
}

static void listele_geri(const char *yol, int boyut, void *baglam) {
    (void)baglam;
    char satir[128];
    if (boyut < 0) snprintf(satir, sizeof satir, "  %s (dizin)\n", yol);
    else           snprintf(satir, sizeof satir, "  %s (%d bayt)\n", yol, boyut);
    sys_yaz(satir);
}

bool modul_dosya(const char *ad, Deger *arg, int n, Deger *cikti) {
    char tampon[TDS_AZAMI_YOL];

    if (strcmp(ad, "yaz") == 0) {
        const char *yol = yolu_coz(metin_arg(arg, n, 0), tampon, sizeof tampon);
        const char *ic = metin_arg(arg, n, 1);
        int r = tds_yaz(yol, ic, strlen(ic));
        *cikti = deger_sayi(r);
        return true;
    }
    if (strcmp(ad, "ekle") == 0) {
        const char *yol = yolu_coz(metin_arg(arg, n, 0), tampon, sizeof tampon);
        const char *ic = metin_arg(arg, n, 1);
        int r = tds_ekle(yol, ic, strlen(ic));
        *cikti = deger_sayi(r);
        return true;
    }
    if (strcmp(ad, "oku") == 0) {
        const char *yol = yolu_coz(metin_arg(arg, n, 0), tampon, sizeof tampon);
        char ic[DEGER_METIN_AZAMI];
        int r = tds_oku(yol, ic, sizeof ic - 1);
        if (r < 0) { *cikti = deger_bos(); return true; }
        if (r >= (int)sizeof ic) r = sizeof ic - 1;
        ic[r] = '\0';
        *cikti = deger_metin(ic);
        return true;
    }
    if (strcmp(ad, "sil") == 0) {
        const char *yol = yolu_coz(metin_arg(arg, n, 0), tampon, sizeof tampon);
        *cikti = deger_mantik(tds_sil(yol) == 0);
        return true;
    }
    if (strcmp(ad, "var") == 0) {
        const char *yol = yolu_coz(metin_arg(arg, n, 0), tampon, sizeof tampon);
        *cikti = deger_mantik(tds_var(yol));
        return true;
    }
    if (strcmp(ad, "listele") == 0) {
        const char *dizin = (n > 0 && arg[0].tur == DEG_METIN) ? arg[0].metin : "/belgeler";
        int sayi = tds_listele(dizin, listele_geri, NULL);
        *cikti = deger_sayi(sayi);
        return true;
    }
    return false;
}
