/* agac.c — AST dugum ayirma/serbest birakma (cekirdek bellek ayiricisi uzerine). */
#include "agac.h"
#include "../cekirdek/bellek.h"
#include <string.h>

Dugum *agac_dugum(DugumTuru tur) {
    Dugum *d = (Dugum *)bellek_ayir(sizeof(Dugum));
    if (!d) return NULL;
    memset(d, 0, sizeof(Dugum));
    d->tur = tur;
    return d;
}

void agac_serbest(Dugum *d) {
    while (d) {
        agac_serbest(d->sol);
        agac_serbest(d->sag);
        agac_serbest(d->govde);
        agac_serbest(d->govde2);
        agac_serbest(d->parametreler);
        agac_serbest(d->argumanlar);
        Dugum *sonraki = d->sonraki;
        bellek_birak(d);
        d = sonraki; /* zinciri dongu ile coz (derin ozyineyi azalt) */
    }
}
