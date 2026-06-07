/* dizi.c — dizi islemleri (cekirdek bellek ayiricisi uzerine). */
#include "dizi.h"
#include "../cekirdek/bellek.h"
#include <string.h>

#define DIZI_BASLANGIC_KAPASITE 4

Dizi *dizi_olustur(void) {
    Dizi *d = (Dizi *)bellek_ayir(sizeof(Dizi));
    if (!d) return NULL;
    d->kapasite = DIZI_BASLANGIC_KAPASITE;
    d->sayi = 0;
    d->ogeler = (Deger *)bellek_ayir(sizeof(Deger) * d->kapasite);
    if (!d->ogeler) { d->kapasite = 0; }
    return d;
}

static bool buyut(Dizi *d) {
    int yeni = d->kapasite < DIZI_BASLANGIC_KAPASITE ? DIZI_BASLANGIC_KAPASITE
                                                     : d->kapasite * 2;
    Deger *p = (Deger *)bellek_yeniden(d->ogeler, sizeof(Deger) * yeni);
    if (!p) return false;
    d->ogeler = p;
    d->kapasite = yeni;
    return true;
}

void dizi_ekle(Dizi *d, Deger v) {
    if (!d) return;
    if (d->sayi >= d->kapasite && !buyut(d)) return;
    d->ogeler[d->sayi++] = v;
}

Deger dizi_al(Dizi *d, int i) {
    if (!d || i < 0 || i >= d->sayi) return deger_bos();
    return d->ogeler[i];
}

bool dizi_ata(Dizi *d, int i, Deger v) {
    if (!d || i < 0 || i >= d->sayi) return false;
    d->ogeler[i] = v;
    return true;
}

Deger dizi_cikar(Dizi *d) {
    if (!d || d->sayi == 0) return deger_bos();
    return d->ogeler[--d->sayi];
}

int dizi_uzunluk(Dizi *d) { return d ? d->sayi : 0; }
