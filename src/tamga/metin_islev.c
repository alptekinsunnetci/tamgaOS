/* metin_islev.c — metin yerlesikleri (parça/bul/içerir/.../parçala/birleştir). */
#include "metin_islev.h"
#include "dizi.h"
#include <string.h>

/* arg i bir metin mi? degilse "" dondur. */
static const char *m_arg(Deger *arg, int n, int i) {
    return (i < n && arg[i].tur == DEG_METIN) ? arg[i].metin : "";
}

bool metin_yerlesik(const char *ad, Deger *arg, int n, Deger *cikti) {

    if (strcmp(ad, "parça") == 0 || strcmp(ad, "parca") == 0) {
        const char *s = m_arg(arg, n, 0);
        int len = (int)strlen(s);
        int b = (n > 1 && arg[1].tur == DEG_SAYI) ? (int)arg[1].sayi : 0;
        int u = (n > 2 && arg[2].tur == DEG_SAYI) ? (int)arg[2].sayi : len - b;
        if (b < 0) b = 0;
        if (b > len) b = len;
        if (u < 0) u = 0;
        if (b + u > len) u = len - b;
        if (u > DEGER_METIN_AZAMI - 1) u = DEGER_METIN_AZAMI - 1;
        char out[DEGER_METIN_AZAMI];
        memcpy(out, s + b, u); out[u] = '\0';
        *cikti = deger_metin(out);
        return true;
    }

    if (strcmp(ad, "bul") == 0) {
        const char *s = m_arg(arg, n, 0), *alt = m_arg(arg, n, 1);
        const char *p = strstr(s, alt);
        *cikti = deger_sayi(p ? (double)(p - s) : -1.0);
        return true;
    }

    if (strcmp(ad, "içerir") == 0 || strcmp(ad, "icerir") == 0) {
        *cikti = deger_mantik(strstr(m_arg(arg, n, 0), m_arg(arg, n, 1)) != NULL);
        return true;
    }

    if (strcmp(ad, "başlar") == 0 || strcmp(ad, "baslar") == 0) {
        const char *s = m_arg(arg, n, 0), *on = m_arg(arg, n, 1);
        *cikti = deger_mantik(strncmp(s, on, strlen(on)) == 0);
        return true;
    }

    if (strcmp(ad, "biter") == 0) {
        const char *s = m_arg(arg, n, 0), *son = m_arg(arg, n, 1);
        size_t ls = strlen(s), lson = strlen(son);
        *cikti = deger_mantik(ls >= lson && strcmp(s + ls - lson, son) == 0);
        return true;
    }

    if (strcmp(ad, "kırp") == 0 || strcmp(ad, "kirp") == 0) {
        const char *s = m_arg(arg, n, 0);
        while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') s++;
        int len = (int)strlen(s);
        while (len > 0 && (s[len-1]==' '||s[len-1]=='\t'||s[len-1]=='\r'||s[len-1]=='\n')) len--;
        if (len > DEGER_METIN_AZAMI - 1) len = DEGER_METIN_AZAMI - 1;
        char out[DEGER_METIN_AZAMI];
        memcpy(out, s, len); out[len] = '\0';
        *cikti = deger_metin(out);
        return true;
    }

    if (strcmp(ad, "değiştir") == 0 || strcmp(ad, "degistir") == 0) {
        const char *s = m_arg(arg, n, 0), *eski = m_arg(arg, n, 1), *yeni = m_arg(arg, n, 2);
        char out[DEGER_METIN_AZAMI];
        int o = 0; size_t le = strlen(eski);
        if (le == 0) { *cikti = deger_metin(s); return true; }
        for (const char *p = s; *p && o < DEGER_METIN_AZAMI - 1; ) {
            if (strncmp(p, eski, le) == 0) {
                for (const char *y = yeni; *y && o < DEGER_METIN_AZAMI - 1; y++) out[o++] = *y;
                p += le;
            } else {
                out[o++] = *p++;
            }
        }
        out[o] = '\0';
        *cikti = deger_metin(out);
        return true;
    }

    if (strcmp(ad, "parçala") == 0 || strcmp(ad, "parcala") == 0) {
        const char *s = m_arg(arg, n, 0), *ayrac = m_arg(arg, n, 1);
        Dizi *dz = dizi_olustur();
        size_t la = strlen(ayrac);
        if (la == 0) { dizi_ekle(dz, deger_metin(s)); *cikti = deger_dizi(dz); return true; }
        const char *bas = s, *p;
        char parca[DEGER_METIN_AZAMI];
        while ((p = strstr(bas, ayrac)) != NULL) {
            int u = (int)(p - bas);
            if (u > DEGER_METIN_AZAMI - 1) u = DEGER_METIN_AZAMI - 1;
            memcpy(parca, bas, u); parca[u] = '\0';
            dizi_ekle(dz, deger_metin(parca));
            bas = p + la;
        }
        dizi_ekle(dz, deger_metin(bas));   /* son parça */
        *cikti = deger_dizi(dz);
        return true;
    }

    if (strcmp(ad, "birleştir") == 0 || strcmp(ad, "birlestir") == 0) {
        if (n < 1 || arg[0].tur != DEG_DIZI) { *cikti = deger_metin(""); return true; }
        Dizi *dz = (Dizi *)arg[0].dizi;
        const char *ayrac = (n > 1 && arg[1].tur == DEG_METIN) ? arg[1].metin : "";
        char out[DEGER_METIN_AZAMI]; int o = 0;
        int m = dizi_uzunluk(dz);
        for (int i = 0; i < m && o < DEGER_METIN_AZAMI - 1; i++) {
            if (i > 0) for (const char *y = ayrac; *y && o < DEGER_METIN_AZAMI - 1; y++) out[o++] = *y;
            char oge[DEGER_METIN_AZAMI];
            deger_yazdir(dizi_al(dz, i), oge, sizeof oge);
            for (const char *y = oge; *y && o < DEGER_METIN_AZAMI - 1; y++) out[o++] = *y;
        }
        out[o] = '\0';
        *cikti = deger_metin(out);
        return true;
    }

    return false; /* metin yerlesigi degil */
}
