/* olay.c — sabit kapasiteli halka tampon. */
#include "olay.h"

#define OLAY_KAPASITE 32

static Olay   s_kuyruk[OLAY_KAPASITE];
static int    s_bas;   /* okuma */
static int    s_son;   /* yazma */
static int    s_sayi;

void olay_baslat(void) { s_bas = s_son = s_sayi = 0; }

bool olay_gonder(OlayTuru tur, uint32_t veri) {
    if (s_sayi >= OLAY_KAPASITE) return false;
    s_kuyruk[s_son].tur = tur;
    s_kuyruk[s_son].veri = veri;
    s_son = (s_son + 1) % OLAY_KAPASITE;
    s_sayi++;
    return true;
}

bool olay_al(Olay *cikti) {
    if (s_sayi == 0) return false;
    *cikti = s_kuyruk[s_bas];
    s_bas = (s_bas + 1) % OLAY_KAPASITE;
    s_sayi--;
    return true;
}
