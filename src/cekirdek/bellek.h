/* bellek.h — Tamga cekirdek bellek yoneticisi.
 * Sabit statik arena uzerinde basit serbest-listeli ayirici. RP2040'in
 * sinirli RAM'ine (264 KB) saygi duyar; libc malloc'a bagimli degildir.
 */
#ifndef TAMGA_BELLEK_H
#define TAMGA_BELLEK_H

#include <stddef.h>

void   bellek_baslat(void);            /* arenayi sifirla */
void  *bellek_ayir(size_t boyut);      /* blok ayir (8'e hizali), yoksa NULL */
void   bellek_birak(void *isaretci);   /* blogu geri ver */
void  *bellek_yeniden(void *isaretci, size_t yeni_boyut); /* realloc benzeri */

size_t bellek_toplam(void);            /* arena toplam bayt */
size_t bellek_kullanilan(void);        /* su an kullanimda olan bayt */
size_t bellek_bos(void);               /* serbest bayt */

#endif /* TAMGA_BELLEK_H */
