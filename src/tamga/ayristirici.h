/* ayristirici.h — Tamga parser (ozyineli inis). Belirtec akisini AST'ye cevirir. */
#ifndef TAMGA_AYRISTIRICI_H
#define TAMGA_AYRISTIRICI_H

#include "tarayici.h"
#include "agac.h"
#include <stdbool.h>

typedef struct {
    Tarayici tar;
    Belirtec simdi;
    Belirtec ileri;     /* bir belirtec ileri bakis */
    bool     hata;
    char     mesaj[160];
    int      hata_satir;
} Ayristirici;

void   ayristirici_baslat(Ayristirici *a, const char *kaynak);
Dugum *ayristir(Ayristirici *a);   /* programi parse et; deyim zinciri dondur (hata -> NULL) */

#endif /* TAMGA_AYRISTIRICI_H */
