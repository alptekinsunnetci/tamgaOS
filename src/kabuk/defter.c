/* defter.c — Tamga Defteri: satir tabanli metin editoru.
 *
 * Klasik 'ed' ruhunda modal calisir: komut modunda komut girilir; 'ekle'
 * insert moduna gecer ve tek basina '.' satiri yazana kadar girilen satirlar
 * sona eklenir. Tam-ekran kontrol (ANSI/raw) gerektirmez; her seri terminalde
 * ve pipe ile calisir.
 */
#include "defter.h"
#include "../hal/hal.h"
#include "../tds/tds.h"
#include "../tamga_yapilandirma.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* DEFTER_MAKS_SATIR ve DEFTER_SATIR_UZ: tamga_yapilandirma.h (platforma gore) */

static char s_satir[DEFTER_MAKS_SATIR][DEFTER_SATIR_UZ];
static int  s_sayi;

static void yaz(const char *s) { hal_uart_yaz(s); }

static void icerik_yukle(const char *yol) {
    s_sayi = 0;
    if (!tds_var(yol)) return;
    static char ham[TDS_AZAMI_BOYUT + 1];
    int n = tds_oku(yol, ham, TDS_AZAMI_BOYUT);
    if (n < 0) return;
    ham[n] = '\0';
    /* '\n' ile satirlara bol */
    int j = 0;
    for (int i = 0; i <= n && s_sayi < DEFTER_MAKS_SATIR; i++) {
        if (ham[i] == '\n' || ham[i] == '\0') {
            s_satir[s_sayi][j] = '\0';
            if (ham[i] == '\0') { if (j > 0) s_sayi++; break; }
            s_sayi++;
            j = 0;
        } else if (j < DEFTER_SATIR_UZ - 1) {
            s_satir[s_sayi][j++] = ham[i];
        }
    }
}

static void listele(void) {
    if (s_sayi == 0) { yaz("  (bos)\n"); return; }
    for (int i = 0; i < s_sayi; i++) {
        char b[DEFTER_SATIR_UZ + 16];
        snprintf(b, sizeof b, "%3d | %.*s\n", i + 1, DEFTER_SATIR_UZ - 1, s_satir[i]);
        yaz(b);
    }
}

static void ekle_modu(void) {
    yaz("[ekleme modu — bitirmek icin tek basina '.' yazin]\n");
    char satir[DEFTER_SATIR_UZ];
    for (;;) {
        int u = hal_uart_oku_satir(satir, sizeof satir);
        if (u < 0) break;                       /* girdi sonu */
        if (strcmp(satir, ".") == 0) break;     /* ekleme modunu bitir */
        if (s_sayi >= DEFTER_MAKS_SATIR) { yaz("! satir siniri doldu\n"); break; }
        snprintf(s_satir[s_sayi], DEFTER_SATIR_UZ, "%s", satir);
        s_sayi++;
    }
}

static void sil_satir(int n) {           /* 1 tabanli */
    if (n < 1 || n > s_sayi) { yaz("! gecersiz satir no\n"); return; }
    for (int i = n - 1; i < s_sayi - 1; i++)
        memcpy(s_satir[i], s_satir[i + 1], DEFTER_SATIR_UZ);
    s_sayi--;
}

static void degistir_satir(int n, const char *metin) {  /* 1 tabanli */
    if (n < 1 || n > s_sayi) { yaz("! gecersiz satir no\n"); return; }
    strncpy(s_satir[n - 1], metin ? metin : "", DEFTER_SATIR_UZ - 1);
    s_satir[n - 1][DEFTER_SATIR_UZ - 1] = '\0';
}

static void kaydet(const char *yol) {
    static char ham[TDS_AZAMI_BOYUT + 1];
    int p = 0;
    for (int i = 0; i < s_sayi; i++) {
        int need = (int)strlen(s_satir[i]) + 1;
        if (p + need >= TDS_AZAMI_BOYUT) { yaz("! dosya cok buyuk, kirpildi\n"); break; }
        p += snprintf(ham + p, TDS_AZAMI_BOYUT - p, "%s\n", s_satir[i]);
    }
    if (tds_yaz(yol, ham, p) >= 0) {
        char b[96];
        snprintf(b, sizeof b, "kaydedildi: %s (%d satir, %d bayt)\n", yol, s_sayi, p);
        yaz(b);
    } else {
        yaz("! kaydetme hatasi\n");
    }
}

static void yardim(void) {
    yaz("Defter komutlari:\n");
    yaz("  liste            satirlari numarali goster\n");
    yaz("  ekle             ekleme modu (sona ekle; '.' ile bitir)\n");
    yaz("  sil <n>          n. satiri sil\n");
    yaz("  değiştir <n> <metin>  n. satiri degistir\n");
    yaz("  kaydet           dosyaya yaz\n");
    yaz("  çık              editörden çık\n");
    yaz("  yardım           bu yardim\n");
}

/* Komut satirini ad + kalan'a ayir. kalan, ilk bosluktan sonrasi (trim'siz). */
static char *ilk_kelime(char *satir, char **kalan) {
    char *p = satir;
    while (*p == ' ' || *p == '\t') p++;
    char *ad = p;
    while (*p && *p != ' ' && *p != '\t') p++;
    if (*p) { *p++ = '\0'; while (*p == ' ' || *p == '\t') p++; }
    *kalan = p;
    return ad;
}

void defter_calistir(const char *yol) {
    icerik_yukle(yol);
    char b[128];
    yaz("=== Tamga Defteri ===\n");
    snprintf(b, sizeof b, "dosya: %s (%d satir). 'yardım' yazın.\n", yol, s_sayi);
    yaz(b);

    char satir[DEFTER_SATIR_UZ + 32];
    bool degisti = false;
    for (;;) {
        yaz("defter> ");
        int u = hal_uart_oku_satir(satir, sizeof satir);
        if (u < 0) break;
        if (u == 0) continue;

        char *kalan;
        char *k = ilk_kelime(satir, &kalan);

        if (strcmp(k, "liste") == 0 || strcmp(k, "l") == 0) {
            listele();
        } else if (strcmp(k, "ekle") == 0 || strcmp(k, "i") == 0) {
            ekle_modu(); degisti = true;
        } else if (strcmp(k, "sil") == 0) {
            sil_satir(atoi(kalan)); degisti = true;
        } else if (strcmp(k, "değiştir") == 0 || strcmp(k, "degistir") == 0) {
            char *kalan2; char *no = ilk_kelime(kalan, &kalan2);
            degistir_satir(atoi(no), kalan2); degisti = true;
        } else if (strcmp(k, "kaydet") == 0 || strcmp(k, "k") == 0) {
            kaydet(yol); degisti = false;
        } else if (strcmp(k, "çık") == 0 || strcmp(k, "cik") == 0 || strcmp(k, "q") == 0) {
            if (degisti) yaz("(kaydedilmemiş değişiklikler var — 'kaydet' veya tekrar 'çık')\n"),
                         degisti = false;
            else break;
        } else if (strcmp(k, "yardım") == 0 || strcmp(k, "yardim") == 0 || strcmp(k, "?") == 0) {
            yardim();
        } else {
            yaz("bilinmeyen komut. 'yardım' yazın.\n");
        }
    }
    yaz("defterden çıkıldı.\n");
}
