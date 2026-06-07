/* yorumlayici.h — agac-yuruyen Tamga yorumlayicisi.
 * AST'yi dogrudan calistirir; kuresel kapsami tutar.
 */
#ifndef TAMGA_YORUMLAYICI_H
#define TAMGA_YORUMLAYICI_H

#include "agac.h"
#include "ortam.h"
#include "deger.h"
#include <stdbool.h>

typedef enum { SINYAL_YOK = 0, SINYAL_DUR, SINYAL_DEVAM } DonguSinyali;

typedef struct {
    Ortam kuresel;
    bool  donduruyor;        /* döndür sinyali */
    int   dongu_sinyali;     /* DonguSinyali: dur/devam */
    Deger donen;
    bool  hata;
    char  mesaj[160];
} Yorumlayici;

void yorumlayici_baslat(Yorumlayici *y);

/* Programi calistir. Hata olursa false ve y->mesaj doldurulur. */
bool yorumla(Yorumlayici *y, Dugum *program);

/* Tek bir ifadeyi degerlendir (REPL icin pratik). */
Deger yorumla_ifade(Yorumlayici *y, Ortam *o, Dugum *d);

#endif /* TAMGA_YORUMLAYICI_H */
