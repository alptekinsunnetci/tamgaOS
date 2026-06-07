/* ortam.c — kapsam tablosu islemleri. */
#include "ortam.h"
#include <string.h>

void ortam_baslat(Ortam *o, Ortam *ust) {
    o->ust = ust;
    o->sayi = 0;
}

static int yerel_indeks(Ortam *o, const char *ad) {
    for (int i = 0; i < o->sayi; i++)
        if (strcmp(o->adlar[i], ad) == 0) return i;
    return -1;
}

bool ortam_tanimla(Ortam *o, const char *ad, Deger d) {
    int i = yerel_indeks(o, ad);
    if (i >= 0) { o->degerler[i] = d; return true; }
    if (o->sayi >= ORTAM_AZAMI) return false;
    strncpy(o->adlar[o->sayi], ad, ORTAM_AD_AZAMI - 1);
    o->adlar[o->sayi][ORTAM_AD_AZAMI - 1] = '\0';
    o->degerler[o->sayi] = d;
    o->sayi++;
    return true;
}

bool ortam_al(Ortam *o, const char *ad, Deger *cikti) {
    for (Ortam *k = o; k; k = k->ust) {
        int i = yerel_indeks(k, ad);
        if (i >= 0) { *cikti = k->degerler[i]; return true; }
    }
    return false;
}

bool ortam_guncelle(Ortam *o, const char *ad, Deger d) {
    for (Ortam *k = o; k; k = k->ust) {
        int i = yerel_indeks(k, ad);
        if (i >= 0) { k->degerler[i] = d; return true; }
    }
    /* yoksa en yerel kapsamda olustur */
    return ortam_tanimla(o, ad, d);
}
