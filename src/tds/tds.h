/* tds.h — Tamga Dosya Sistemi (TDS) API.
 *
 * Milestone 1: RAM destekli, yol-anahtarli duz dosya tablosu. Ileride ayni
 * arayuz littlefs (Pico flash) uzerine tasinacak. Yollar '/' ile baslar:
 *   /uygulamalar /belgeler /ayarlar /gunlukler /aygitlar /gecici
 */
#ifndef TAMGA_TDS_H
#define TAMGA_TDS_H

#include <stddef.h>
#include <stdbool.h>
#include "../tamga_yapilandirma.h"

#define TDS_AZAMI_YOL   64
/* TDS_AZAMI_DOSYA ve TDS_AZAMI_BOYUT: tamga_yapilandirma.h (platforma gore) */

void   tds_baslat(void);
void   tds_tohum(void);   /* gomulu ornek uygulamalari /uygulamalar altina yukler */

/* Kalicilik (hal_flash blok aygiti uzerine serilestirme). */
bool   tds_yukle(void);   /* flash imajindan tabloyu yukle; gecerli imaj yoksa false */
void   tds_kaydet(void);  /* tabloyu flash imajina yaz + flush */

/* Standart kok dizinler (orn. "/uygulamalar"). dizin_sayisi adet. */
extern const char *const TDS_KOK_DIZINLER[];
extern const int        TDS_KOK_DIZIN_SAYISI;

/* Temel dosya islemleri (dil API'si: dosya.oku/yaz/sil/listele) */
int    tds_yaz(const char *yol, const char *veri, size_t boyut); /* olustur/uzerine yaz */
int    tds_ekle(const char *yol, const char *veri, size_t boyut);
int    tds_oku(const char *yol, char *tampon, size_t boyut);     /* okunan bayt veya -1 */
int    tds_sil(const char *yol);
bool   tds_var(const char *yol);
int    tds_boyut(const char *yol);                               /* bayt veya -1 */

/* Listeleme: bir dizindeki girdileri callback ile gez */
typedef void (*TdsListeFn)(const char *yol, int boyut, void *baglam);
int    tds_listele(const char *dizin, TdsListeFn fn, void *baglam); /* girdi sayisi */

#endif /* TAMGA_TDS_H */
