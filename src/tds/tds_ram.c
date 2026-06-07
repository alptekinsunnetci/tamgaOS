/* tds_ram.c — TDS'nin RAM destekli implementasyonu (host + ilk Pico testleri).
 * Sabit kapasiteli dosya tablosu; her dosya statik tampon kullanir.
 */
#include "tds.h"
#include "../hal/hal.h"
#include <string.h>

typedef struct {
    char  yol[TDS_AZAMI_YOL];
    char  veri[TDS_AZAMI_BOYUT];
    int   boyut;
    bool  kullanimda;
} TdsDosya;

static TdsDosya s_dosyalar[TDS_AZAMI_DOSYA];

/* Standart dizin agaci (her zaman "var" kabul edilir). */
const char *const TDS_KOK_DIZINLER[] = {
    "/uygulamalar", "/belgeler", "/ayarlar",
    "/gunlukler", "/aygitlar", "/gecici"
};
const int TDS_KOK_DIZIN_SAYISI =
    (int)(sizeof(TDS_KOK_DIZINLER) / sizeof(TDS_KOK_DIZINLER[0]));

void tds_baslat(void) {
    for (int i = 0; i < TDS_AZAMI_DOSYA; i++) s_dosyalar[i].kullanimda = false;
}

static TdsDosya *bul(const char *yol) {
    for (int i = 0; i < TDS_AZAMI_DOSYA; i++)
        if (s_dosyalar[i].kullanimda && strcmp(s_dosyalar[i].yol, yol) == 0)
            return &s_dosyalar[i];
    return NULL;
}

static TdsDosya *bos_bul(void) {
    for (int i = 0; i < TDS_AZAMI_DOSYA; i++)
        if (!s_dosyalar[i].kullanimda) return &s_dosyalar[i];
    return NULL;
}

int tds_yaz(const char *yol, const char *veri, size_t boyut) {
    if (!yol || strlen(yol) >= TDS_AZAMI_YOL) return -1;
    if (boyut > TDS_AZAMI_BOYUT) return -1;
    TdsDosya *d = bul(yol);
    if (!d) {
        d = bos_bul();
        if (!d) return -1;
        strcpy(d->yol, yol);
        d->kullanimda = true;
    }
    memcpy(d->veri, veri, boyut);
    d->boyut = (int)boyut;
    tds_kaydet();              /* otomatik kalici kayit */
    return (int)boyut;
}

int tds_ekle(const char *yol, const char *veri, size_t boyut) {
    TdsDosya *d = bul(yol);
    if (!d) return tds_yaz(yol, veri, boyut);
    if (d->boyut + (int)boyut > TDS_AZAMI_BOYUT) return -1;
    memcpy(d->veri + d->boyut, veri, boyut);
    d->boyut += (int)boyut;
    tds_kaydet();
    return d->boyut;
}

int tds_oku(const char *yol, char *tampon, size_t boyut) {
    TdsDosya *d = bul(yol);
    if (!d) return -1;
    int n = d->boyut < (int)boyut ? d->boyut : (int)boyut;
    memcpy(tampon, d->veri, n);
    if (n < (int)boyut) tampon[n] = '\0';
    return n;
}

int tds_sil(const char *yol) {
    TdsDosya *d = bul(yol);
    if (!d) return -1;
    d->kullanimda = false;
    tds_kaydet();
    return 0;
}

bool tds_var(const char *yol) { return bul(yol) != NULL; }

int tds_boyut(const char *yol) {
    TdsDosya *d = bul(yol);
    return d ? d->boyut : -1;
}

/* yol, verilen dizinin dogrudan icinde mi? (alt-dizin dahil etmeden) */
static bool dizinde_mi(const char *yol, const char *dizin) {
    size_t dl = strlen(dizin);
    if (strncmp(yol, dizin, dl) != 0) return false;
    const char *kalan = yol + dl;
    if (*kalan != '/') return false;
    kalan++;
    return strchr(kalan, '/') == NULL; /* alt-dizin yok */
}

int tds_listele(const char *dizin, TdsListeFn fn, void *baglam) {
    int sayi = 0;
    /* Kok dizin: standart alt dizinleri (boyut = -1) listele */
    if (strcmp(dizin, "/") == 0) {
        for (int i = 0; i < TDS_KOK_DIZIN_SAYISI; i++) {
            if (fn) fn(TDS_KOK_DIZINLER[i], -1, baglam);
            sayi++;
        }
    }
    for (int i = 0; i < TDS_AZAMI_DOSYA; i++) {
        if (s_dosyalar[i].kullanimda && dizinde_mi(s_dosyalar[i].yol, dizin)) {
            if (fn) fn(s_dosyalar[i].yol, s_dosyalar[i].boyut, baglam);
            sayi++;
        }
    }
    return sayi;
}

/* --- Kalicilik: hal_flash blok aygiti uzerine basit serilestirme ---
 * Bicim:  ['T''D''S''1'][int32 sayi]  ardindan her dosya icin:
 *         [yol: TDS_AZAMI_YOL bayt][int32 boyut][boyut bayt veri]
 * Yazma sirasinda flash sinirini asarsa iptal edilir (eski imaj korunur).
 */
#define TDS_SIHIR "TDS1"

static int flash_yaz_ilerlet(uint32_t *off, const void *veri, uint32_t n) {
    if (*off + n > hal_flash_boyut()) return -1;
    if (hal_flash_yaz(*off, veri, n) != 0) return -1;
    *off += n;
    return 0;
}

static int flash_oku_ilerlet(uint32_t *off, void *veri, uint32_t n) {
    if (hal_flash_oku(*off, veri, n) != 0) return -1;
    *off += n;
    return 0;
}

void tds_kaydet(void) {
    uint32_t off = 0;
    int32_t sayi = 0;
    for (int i = 0; i < TDS_AZAMI_DOSYA; i++)
        if (s_dosyalar[i].kullanimda) sayi++;

    if (flash_yaz_ilerlet(&off, TDS_SIHIR, 4) != 0) return;
    if (flash_yaz_ilerlet(&off, &sayi, 4) != 0) return;

    for (int i = 0; i < TDS_AZAMI_DOSYA; i++) {
        if (!s_dosyalar[i].kullanimda) continue;
        int32_t b = s_dosyalar[i].boyut;
        if (flash_yaz_ilerlet(&off, s_dosyalar[i].yol, TDS_AZAMI_YOL) != 0) return;
        if (flash_yaz_ilerlet(&off, &b, 4) != 0) return;
        if (b > 0 && flash_yaz_ilerlet(&off, s_dosyalar[i].veri, (uint32_t)b) != 0) return;
    }
    hal_flash_flush();
}

bool tds_yukle(void) {
    uint32_t off = 0;
    char sihir[4];
    if (flash_oku_ilerlet(&off, sihir, 4) != 0) return false;
    if (memcmp(sihir, TDS_SIHIR, 4) != 0) return false;

    int32_t sayi = 0;
    if (flash_oku_ilerlet(&off, &sayi, 4) != 0) return false;
    if (sayi < 0 || sayi > TDS_AZAMI_DOSYA) return false;

    tds_baslat();   /* tabloyu temizle */
    for (int i = 0; i < sayi; i++) {
        TdsDosya *d = &s_dosyalar[i];
        int32_t b = 0;
        if (flash_oku_ilerlet(&off, d->yol, TDS_AZAMI_YOL) != 0) return false;
        if (flash_oku_ilerlet(&off, &b, 4) != 0) return false;
        if (b < 0 || b > TDS_AZAMI_BOYUT) return false;
        if (b > 0 && flash_oku_ilerlet(&off, d->veri, (uint32_t)b) != 0) return false;
        d->boyut = b;
        d->kullanimda = true;
    }
    return true;
}
