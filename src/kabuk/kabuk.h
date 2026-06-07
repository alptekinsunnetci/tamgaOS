/* kabuk.h — Tamga Kabugu (Turkce REPL). */
#ifndef TAMGA_KABUK_H
#define TAMGA_KABUK_H

#include <stdbool.h>

#define KABUK_YOL_AZAMI 64

void kabuk_baslat(void);
void kabuk_gorev(void *baglam);   /* bir REPL turu (cekirdek gorevi olarak) */
bool kabuk_cikis_istendi(void);

#endif /* TAMGA_KABUK_H */
