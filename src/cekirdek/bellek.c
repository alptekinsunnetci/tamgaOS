/* bellek.c — sabit arena uzerinde serbest-listeli ayirici.
 *
 * Her blok bir baslik (Blok) ile baslar. Bloklar bellek adresine gore
 * sirali tek yonlu listede tutulur; bos komsular birlestirilir (coalesce).
 * Amac: gomulu ortamda ongorulebilir, parcalanmaya dayanikli, kucuk ayirici.
 */
#include "bellek.h"
#include "../tamga_yapilandirma.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define HIZA 8u
#define HIZALA(n) (((n) + (HIZA - 1)) & ~(size_t)(HIZA - 1))

typedef struct Blok {
    size_t       boyut;   /* yalniz veri alani (baslik haric) */
    int          bos;     /* 1 = serbest */
    struct Blok *sonraki; /* adres sirali bir sonraki blok */
} Blok;

/* Arena heap'ten bir kez ayrilir (gomulu hedefte statik .bss'i tasirmamak icin;
   ESP32'de WiFi statik .bss'i DRAM'in cogunu alir, heap ise boldur). */
static uint8_t *s_arena;
static Blok    *s_ilk;
static size_t   s_kullanilan;

void bellek_baslat(void) {
    if (!s_arena) {
        s_arena = (uint8_t *)malloc(TAMGA_ARENA_BOYUT);
        if (!s_arena) { s_ilk = NULL; return; }  /* OOM: ayirici devre disi */
    }
    s_ilk = (Blok *)s_arena;
    s_ilk->boyut = TAMGA_ARENA_BOYUT - sizeof(Blok);
    s_ilk->bos = 1;
    s_ilk->sonraki = NULL;
    s_kullanilan = 0;
}

static void belki_bol(Blok *b, size_t istek) {
    /* Kalan, yeni bir blok basligi + en az HIZA kadar veri tutabiliyorsa bol. */
    if (b->boyut >= istek + sizeof(Blok) + HIZA) {
        Blok *yeni = (Blok *)((uint8_t *)(b + 1) + istek);
        yeni->boyut = b->boyut - istek - sizeof(Blok);
        yeni->bos = 1;
        yeni->sonraki = b->sonraki;
        b->boyut = istek;
        b->sonraki = yeni;
    }
}

void *bellek_ayir(size_t boyut) {
    if (boyut == 0) return NULL;
    if (s_ilk == NULL) bellek_baslat();
    size_t istek = HIZALA(boyut);

    for (Blok *b = s_ilk; b; b = b->sonraki) {
        if (b->bos && b->boyut >= istek) {
            belki_bol(b, istek);
            b->bos = 0;
            s_kullanilan += b->boyut + sizeof(Blok);
            return (void *)(b + 1);
        }
    }
    return NULL; /* yer yok */
}

static void birlestir(void) {
    for (Blok *b = s_ilk; b && b->sonraki; ) {
        if (b->bos && b->sonraki->bos) {
            b->boyut += sizeof(Blok) + b->sonraki->boyut;
            b->sonraki = b->sonraki->sonraki;
        } else {
            b = b->sonraki;
        }
    }
}

void bellek_birak(void *isaretci) {
    if (!isaretci) return;
    Blok *b = (Blok *)isaretci - 1;
    if (!b->bos) {
        b->bos = 1;
        s_kullanilan -= b->boyut + sizeof(Blok);
    }
    birlestir();
}

void *bellek_yeniden(void *isaretci, size_t yeni_boyut) {
    if (!isaretci) return bellek_ayir(yeni_boyut);
    if (yeni_boyut == 0) { bellek_birak(isaretci); return NULL; }
    Blok *b = (Blok *)isaretci - 1;
    if (b->boyut >= HIZALA(yeni_boyut)) return isaretci;
    void *yeni = bellek_ayir(yeni_boyut);
    if (!yeni) return NULL;
    memcpy(yeni, isaretci, b->boyut);
    bellek_birak(isaretci);
    return yeni;
}

size_t bellek_toplam(void)      { return TAMGA_ARENA_BOYUT; }
size_t bellek_kullanilan(void)  { return s_kullanilan; }
size_t bellek_bos(void)         { return TAMGA_ARENA_BOYUT - s_kullanilan; }
