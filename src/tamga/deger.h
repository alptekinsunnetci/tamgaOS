/* deger.h — Tamga calisma-zamani deger turleri.
 * Gomulu ortam icin metin, sabit gomulu tampona kopyalanir (heap parcalanmasi
 * olmaz). Islev degeri AST dugumune isaret eder.
 */
#ifndef TAMGA_DEGER_H
#define TAMGA_DEGER_H

#include <stdbool.h>
#include "../tamga_yapilandirma.h"   /* DEGER_METIN_AZAMI (platforma gore) */

typedef enum {
    DEG_BOS,
    DEG_SAYI,
    DEG_MANTIK,
    DEG_METIN,
    DEG_ISLEV,
    DEG_DIZI,
    DEG_SOZLUK
} DegerTuru;

typedef struct {
    DegerTuru tur;
    double    sayi;                       /* DEG_SAYI */
    bool      mantik;                     /* DEG_MANTIK */
    char      metin[DEGER_METIN_AZAMI];   /* DEG_METIN */
    void     *islev;                      /* DEG_ISLEV  -> Dugum* */
    void     *dizi;                       /* DEG_DIZI   -> Dizi* */
    void     *sozluk;                     /* DEG_SOZLUK -> Sozluk* */
} Deger;

Deger deger_bos(void);
Deger deger_sayi(double s);
Deger deger_mantik(bool m);
Deger deger_metin(const char *s);
Deger deger_islev(void *dugum);
Deger deger_dizi(void *dizi);
Deger deger_sozluk(void *sozluk);

bool  deger_dogru_mu(Deger d);                 /* mantiksal dogruluk */
void  deger_yazdir(Deger d, char *tampon, int boyut); /* insan-okur metin */

#endif /* TAMGA_DEGER_H */
