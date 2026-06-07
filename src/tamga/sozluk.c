/* sozluk.c — sözlük islemleri (cekirdek bellek ayiricisi uzerine). */
#include "sozluk.h"
#include "../cekirdek/bellek.h"
#include <string.h>

#define SOZLUK_BASLANGIC 4

Sozluk *sozluk_olustur(void) {
    Sozluk *s = (Sozluk *)bellek_ayir(sizeof(Sozluk));
    if (!s) return NULL;
    s->kapasite = SOZLUK_BASLANGIC;
    s->sayi = 0;
    s->ciftler = (SozlukCift *)bellek_ayir(sizeof(SozlukCift) * s->kapasite);
    if (!s->ciftler) s->kapasite = 0;
    return s;
}

static int indeks(Sozluk *s, const char *anahtar) {
    for (int i = 0; i < s->sayi; i++)
        if (strcmp(s->ciftler[i].anahtar, anahtar) == 0) return i;
    return -1;
}

static bool buyut(Sozluk *s) {
    int yeni = s->kapasite < SOZLUK_BASLANGIC ? SOZLUK_BASLANGIC : s->kapasite * 2;
    SozlukCift *p = (SozlukCift *)bellek_yeniden(s->ciftler, sizeof(SozlukCift) * yeni);
    if (!p) return false;
    s->ciftler = p;
    s->kapasite = yeni;
    return true;
}

bool sozluk_ata(Sozluk *s, const char *anahtar, Deger deger) {
    if (!s || !anahtar) return false;
    int i = indeks(s, anahtar);
    if (i >= 0) { s->ciftler[i].deger = deger; return true; }
    if (s->sayi >= s->kapasite && !buyut(s)) return false;
    SozlukCift *c = &s->ciftler[s->sayi++];
    strncpy(c->anahtar, anahtar, SOZLUK_ANAHTAR_UZ - 1);
    c->anahtar[SOZLUK_ANAHTAR_UZ - 1] = '\0';
    c->deger = deger;
    return true;
}

Deger sozluk_al(Sozluk *s, const char *anahtar) {
    if (!s) return deger_bos();
    int i = indeks(s, anahtar);
    return i >= 0 ? s->ciftler[i].deger : deger_bos();
}

bool sozluk_var(Sozluk *s, const char *anahtar) {
    return s && indeks(s, anahtar) >= 0;
}

bool sozluk_sil(Sozluk *s, const char *anahtar) {
    if (!s) return false;
    int i = indeks(s, anahtar);
    if (i < 0) return false;
    for (int j = i; j < s->sayi - 1; j++) s->ciftler[j] = s->ciftler[j + 1];
    s->sayi--;
    return true;
}

int sozluk_uzunluk(Sozluk *s) { return s ? s->sayi : 0; }

const char *sozluk_anahtar(Sozluk *s, int i) {
    if (!s || i < 0 || i >= s->sayi) return NULL;
    return s->ciftler[i].anahtar;
}
