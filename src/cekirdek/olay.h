/* olay.h — basit ring-buffer mesaj kuyrugu (cekirdek olay sistemi). */
#ifndef TAMGA_OLAY_H
#define TAMGA_OLAY_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    OLAY_YOK = 0,
    OLAY_TUS,        /* veri = tus kodu */
    OLAY_ZAMANLAYICI,
    OLAY_GPIO,
    OLAY_WIFI
} OlayTuru;

typedef struct {
    OlayTuru tur;
    uint32_t veri;
} Olay;

void  olay_baslat(void);
bool  olay_gonder(OlayTuru tur, uint32_t veri); /* dolu ise false */
bool  olay_al(Olay *cikti);                     /* bos ise false */

#endif /* TAMGA_OLAY_H */
