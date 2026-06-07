/* zamanlayici.c — kooperatif round-robin gorev tablosu. */
#include "zamanlayici.h"

#define MAKS_GOREV 8

typedef struct {
    const char *ad;
    GorevFn     fn;
    void       *baglam;
    bool        aktif;
} Gorev;

static Gorev s_gorevler[MAKS_GOREV];
static int   s_sayi;

void zamanlayici_baslat(void) {
    for (int i = 0; i < MAKS_GOREV; i++) s_gorevler[i].aktif = false;
    s_sayi = 0;
}

int gorev_ekle(const char *ad, GorevFn fn, void *baglam) {
    for (int i = 0; i < MAKS_GOREV; i++) {
        if (!s_gorevler[i].aktif) {
            s_gorevler[i].ad = ad;
            s_gorevler[i].fn = fn;
            s_gorevler[i].baglam = baglam;
            s_gorevler[i].aktif = true;
            if (i + 1 > s_sayi) s_sayi = i + 1;
            return i;
        }
    }
    return -1;
}

void gorev_durdur(int indeks) {
    if (indeks >= 0 && indeks < MAKS_GOREV) s_gorevler[indeks].aktif = false;
}

void zamanlayici_tik(void) {
    for (int i = 0; i < s_sayi; i++)
        if (s_gorevler[i].aktif && s_gorevler[i].fn)
            s_gorevler[i].fn(s_gorevler[i].baglam);
}

int gorev_sayisi(void) {
    int n = 0;
    for (int i = 0; i < s_sayi; i++) if (s_gorevler[i].aktif) n++;
    return n;
}
