/* sozluk.h — Tamga sözlük (anahtar-değer) veri yapisi.
 * Diziler gibi BASVURU turudur (Deger icinde isaretci). Anahtarlar metindir.
 * Bellek cekirdek ayiricisindan (bellek_ayir) gelir.
 */
#ifndef TAMGA_SOZLUK_H
#define TAMGA_SOZLUK_H

#include "deger.h"
#include <stdbool.h>

#define SOZLUK_ANAHTAR_UZ 40

typedef struct {
    char  anahtar[SOZLUK_ANAHTAR_UZ];
    Deger deger;
} SozlukCift;

typedef struct {
    SozlukCift *ciftler;
    int         sayi;
    int         kapasite;
} Sozluk;

Sozluk *sozluk_olustur(void);
bool    sozluk_ata(Sozluk *s, const char *anahtar, Deger deger);  /* ekle/guncelle */
Deger   sozluk_al(Sozluk *s, const char *anahtar);                /* yoksa boş */
bool    sozluk_var(Sozluk *s, const char *anahtar);
bool    sozluk_sil(Sozluk *s, const char *anahtar);
int     sozluk_uzunluk(Sozluk *s);
const char *sozluk_anahtar(Sozluk *s, int i);                     /* i. anahtar veya NULL */

#endif /* TAMGA_SOZLUK_H */
