/* deger.c — deger olusturucular ve donusumler. */
#include "deger.h"
#include "dizi.h"
#include "sozluk.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

Deger deger_bos(void) {
    Deger d; memset(&d, 0, sizeof d); d.tur = DEG_BOS; return d;
}
Deger deger_sayi(double s) {
    Deger d; memset(&d, 0, sizeof d); d.tur = DEG_SAYI; d.sayi = s; return d;
}
Deger deger_mantik(bool m) {
    Deger d; memset(&d, 0, sizeof d); d.tur = DEG_MANTIK; d.mantik = m; return d;
}
Deger deger_metin(const char *s) {
    Deger d; memset(&d, 0, sizeof d); d.tur = DEG_METIN;
    if (s) { strncpy(d.metin, s, DEGER_METIN_AZAMI - 1); d.metin[DEGER_METIN_AZAMI - 1] = '\0'; }
    return d;
}
Deger deger_islev(void *dugum) {
    Deger d; memset(&d, 0, sizeof d); d.tur = DEG_ISLEV; d.islev = dugum; return d;
}
Deger deger_dizi(void *dizi) {
    Deger d; memset(&d, 0, sizeof d); d.tur = DEG_DIZI; d.dizi = dizi; return d;
}
Deger deger_sozluk(void *sozluk) {
    Deger d; memset(&d, 0, sizeof d); d.tur = DEG_SOZLUK; d.sozluk = sozluk; return d;
}

bool deger_dogru_mu(Deger d) {
    switch (d.tur) {
        case DEG_BOS:    return false;
        case DEG_SAYI:   return d.sayi != 0.0;
        case DEG_MANTIK: return d.mantik;
        case DEG_METIN:  return d.metin[0] != '\0';
        case DEG_ISLEV:  return true;
        case DEG_DIZI:   return dizi_uzunluk((Dizi *)d.dizi) > 0;
        case DEG_SOZLUK: return sozluk_uzunluk((Sozluk *)d.sozluk) > 0;
    }
    return false;
}

/* Sayiyi gereksiz ".0" olmadan yazdir (tam sayi ise tam sayi gibi). */
static void sayi_yaz(double s, char *tampon, int boyut) {
    if (isnan(s)) { snprintf(tampon, boyut, "tanimsiz"); return; }
    if (s == (long long)s && fabs(s) < 1e15)
        snprintf(tampon, boyut, "%lld", (long long)s);
    else
        snprintf(tampon, boyut, "%g", s);
}

void deger_yazdir(Deger d, char *tampon, int boyut) {
    switch (d.tur) {
        case DEG_BOS:    snprintf(tampon, boyut, "boş"); break;
        case DEG_SAYI:   sayi_yaz(d.sayi, tampon, boyut); break;
        case DEG_MANTIK: snprintf(tampon, boyut, d.mantik ? "doğru" : "yanlış"); break;
        case DEG_METIN:  snprintf(tampon, boyut, "%s", d.metin); break;
        case DEG_ISLEV:  snprintf(tampon, boyut, "<işlev>"); break;
        case DEG_DIZI: {
            Dizi *dz = (Dizi *)d.dizi;
            int n = dizi_uzunluk(dz);
            int p = 0;
            p += snprintf(tampon + p, boyut - p, "[");
            for (int i = 0; i < n && p < boyut - 2; i++) {
                char oge[64];
                deger_yazdir(dizi_al(dz, i), oge, sizeof oge);
                p += snprintf(tampon + p, boyut - p, "%s%s", (i ? ", " : ""), oge);
            }
            snprintf(tampon + p, boyut - p, "]");
            break;
        }
        case DEG_SOZLUK: {
            Sozluk *sz = (Sozluk *)d.sozluk;
            int n = sozluk_uzunluk(sz);
            int p = 0;
            p += snprintf(tampon + p, boyut - p, "{");
            for (int i = 0; i < n && p < boyut - 2; i++) {
                char oge[80];
                deger_yazdir(sozluk_al(sz, sozluk_anahtar(sz, i)), oge, sizeof oge);
                p += snprintf(tampon + p, boyut - p, "%s\"%s\": %s",
                              (i ? ", " : ""), sozluk_anahtar(sz, i), oge);
            }
            snprintf(tampon + p, boyut - p, "}");
            break;
        }
        default:         snprintf(tampon, boyut, "?"); break;
    }
}
