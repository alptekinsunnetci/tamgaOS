/* dizi.h — Tamga dizi (array) veri yapisi.
 * Diziler basvuru (reference) turudur: Deger icinde isaretci tutulur; kopyalama
 * sig kopyadir, bu yuzden 'ekle' gibi islemler paylasilan diziyi degistirir.
 * Bellek cekirdek ayiricisindan (bellek_ayir) alinir.
 */
#ifndef TAMGA_DIZI_H
#define TAMGA_DIZI_H

#include "deger.h"
#include <stdbool.h>

typedef struct {
    Deger *ogeler;
    int    sayi;
    int    kapasite;
} Dizi;

Dizi *dizi_olustur(void);
void  dizi_ekle(Dizi *d, Deger v);     /* sona ekle (gerekirse buyut) */
Deger dizi_al(Dizi *d, int i);         /* aralik disi -> boş */
bool  dizi_ata(Dizi *d, int i, Deger v); /* aralik disi -> false */
Deger dizi_cikar(Dizi *d);             /* sondaki ogeyi cikar; bos -> boş */
int   dizi_uzunluk(Dizi *d);

#endif /* TAMGA_DIZI_H */
