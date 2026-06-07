/* tarayici.h — Tamga lexer (tarayici): kaynak metni belirtec akisina cevirir. */
#ifndef TAMGA_TARAYICI_H
#define TAMGA_TARAYICI_H

#include <stddef.h>
#include "belirtec.h"

typedef struct {
    const char *kaynak;
    size_t      konum;
    int         satir;
} Tarayici;

void     tarayici_baslat(Tarayici *t, const char *kaynak);
Belirtec tarayici_sonraki(Tarayici *t);   /* bir sonraki belirteci dondur */

#endif /* TAMGA_TARAYICI_H */
