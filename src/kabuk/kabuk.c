/* kabuk.c — Turkce komut satiri (REPL).
 * Yerlesik komutlari komutlar.c'ye yonlendirir; geri kalan satirlari kalici
 * bir Tamga yorumlayici ortaminda calistirir (degiskenler oturum boyu yasar).
 */
#include "kabuk.h"
#include "komutlar.h"
#include "../hal/hal.h"
#include "../tamga/tamga.h"
#include <string.h>
#include <stdio.h>

#define SATIR_AZAMI 256
#define ARG_AZAMI   16

static Yorumlayici s_yorum;
static bool        s_cikis;
static char        s_cwd[KABUK_YOL_AZAMI];

void kabuk_baslat(void) {
    yorumlayici_baslat(&s_yorum);
    s_cikis = false;
    strcpy(s_cwd, "/");
    hal_uart_yaz("\n");
    hal_uart_yaz("==========================================\n");
    hal_uart_yaz("   TAMGA OS — Türkçe İşletim Sistemi\n");
    hal_uart_yaz("   Sürüm 0.1 (dikey dilim)\n");
    hal_uart_yaz("   'yardım' yazarak komutları görün.\n");
    hal_uart_yaz("==========================================\n");
}

bool kabuk_cikis_istendi(void) { return s_cikis; }

static void istem_yaz(void) {
    char istem[128];
    snprintf(istem, sizeof istem, "tamga@cihaz:%s$ ", s_cwd);
    hal_uart_yaz(istem);
}

/* Satiri bosluklardan argv'ye boler (yerinde degistirir). argc doner. */
static int bol(char *satir, char **argv, int azami) {
    int argc = 0;
    char *p = satir;
    while (*p && argc < azami) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        argv[argc++] = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) *p++ = '\0';
    }
    return argc;
}

void kabuk_gorev(void *baglam) {
    (void)baglam;
    char satir[SATIR_AZAMI];

    istem_yaz();
    int u = hal_uart_oku_satir(satir, sizeof satir);
    if (u < 0) { s_cikis = true; return; }       /* girdi sonu */
    if (u == 0) return;                            /* bos satir */

    /* Yerlesik komut mu? (satirin bir kopyasini bol; orijinali yorumlayici icin sakli tut) */
    char kopya[SATIR_AZAMI];
    strncpy(kopya, satir, sizeof kopya - 1);
    kopya[sizeof kopya - 1] = '\0';
    char *argv[ARG_AZAMI];
    int argc = bol(kopya, argv, ARG_AZAMI);
    if (argc == 0) return;

    if (komut_calistir(argc, argv, s_cwd, &s_cikis))
        return; /* yerlesik komut isledi */

    /* Aksi halde Tamga koduna gonder (REPL: degiskenler kalir) */
    if (!tamga_calistir_ortamda(&s_yorum, satir, true)) {
        hal_uart_yaz("HATA: ");
        hal_uart_yaz(s_yorum.mesaj);
        hal_uart_yaz("\n");
    }
}
