/* ortam.h — degisken kapsami (environment).
 * Sabit boyutlu ad/deger tablosu + bir ust kapsam isaretcisi (islev cagrilari
 * yeni bir ortam acar, kuresel ortami ust olarak gosterir).
 */
#ifndef TAMGA_ORTAM_H
#define TAMGA_ORTAM_H

#include "deger.h"
#include <stdbool.h>
#include "../tamga_yapilandirma.h"   /* ORTAM_AZAMI, ORTAM_AD_AZAMI (platforma gore) */

typedef struct Ortam {
    struct Ortam *ust;
    char  adlar[ORTAM_AZAMI][ORTAM_AD_AZAMI];
    Deger degerler[ORTAM_AZAMI];
    int   sayi;
} Ortam;

void  ortam_baslat(Ortam *o, Ortam *ust);
bool  ortam_tanimla(Ortam *o, const char *ad, Deger d);  /* yerel kapsamda ata */
bool  ortam_al(Ortam *o, const char *ad, Deger *cikti);  /* ust kapsamlara bakar */
bool  ortam_guncelle(Ortam *o, const char *ad, Deger d); /* var olani gunceller */

#endif /* TAMGA_ORTAM_H */
