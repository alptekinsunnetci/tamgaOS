/* komutlar.c — kabuk yerlesik komutlarinin govdesi. */
#include "komutlar.h"
#include "kabuk.h"
#include "defter.h"
#include "../hal/hal.h"
#include "../tds/tds.h"
#include "../tamga/tamga.h"
#include "../cekirdek/bellek.h"
#include "../cekirdek/zamanlayici.h"
#include <string.h>
#include <stdio.h>

/* Yalniz host'ta gercek dosya sisteminden .tam yuklenir; gomulu hedeflerde
   (Pico/ESP32) host FS yoktur, uygulamalar TDS'ten gelir. */
#if !defined(TAMGA_PICO) && !defined(TAMGA_ESP32) && !defined(ARDUINO)
#define TAMGA_HOST_FS 1
#endif

#define KAYNAK_AZAMI 4096

static void yaz(const char *s) { hal_uart_yaz(s); }

/* --- yardimcilar --- */

/* Goreli yolu cwd ile birlestir (basit: "/a" mutlak, aksi cwd ekli). */
static void yol_coz(const char *cwd, const char *girdi, char *cikti, int boyut) {
    if (girdi[0] == '/') { snprintf(cikti, boyut, "%s", girdi); return; }
    if (strcmp(cwd, "/") == 0) snprintf(cikti, boyut, "/%s", girdi);
    else                       snprintf(cikti, boyut, "%s/%s", cwd, girdi);
}

/* .tam kaynagini yukle: once host dosya sistemi, sonra TDS. */
static bool kaynak_yukle(const char *ad, char *tampon, int boyut) {
#if defined(TAMGA_HOST_FS)
    /* 1) verilen yol/ad dogrudan (yalniz host) */
    const char *adaylar[3];
    char yol1[128], yol2[128];
    adaylar[0] = ad;
    snprintf(yol1, sizeof yol1, "uygulamalar/%s", ad);   adaylar[1] = yol1;
    snprintf(yol2, sizeof yol2, "uygulamalar/%s.tam", ad); adaylar[2] = yol2;
    for (int i = 0; i < 3; i++) {
        FILE *f = fopen(adaylar[i], "rb");
        if (f) {
            int n = (int)fread(tampon, 1, boyut - 1, f);
            fclose(f);
            tampon[n] = '\0';
            return true;
        }
    }
#endif
    /* 2) TDS: /uygulamalar/<ad> ve /uygulamalar/<ad>.tam */
    char tyol[TDS_AZAMI_YOL];
    const char *bicim[2] = { "/uygulamalar/%s", "/uygulamalar/%s.tam" };
    for (int i = 0; i < 2; i++) {
        snprintf(tyol, sizeof tyol, bicim[i], ad);
        if (tds_var(tyol)) {
            int n = tds_oku(tyol, tampon, boyut - 1);
            if (n >= 0) { tampon[n] = '\0'; return true; }
        }
    }
    return false;
}

static void listele_geri(const char *yol, int boyut, void *baglam) {
    int *say = (int *)baglam;
    char satir[128];
    if (boyut < 0) snprintf(satir, sizeof satir, "  %-32s <dizin>\n", yol);
    else           snprintf(satir, sizeof satir, "  %-32s %6d bayt\n", yol, boyut);
    yaz(satir);
    (*say)++;
}

/* --- komutlar --- */

static void k_yardim(void) {
    yaz("Tamga OS komutlari:\n");
    yaz("  yardım              bu listeyi goster\n");
    yaz("  listele [dizin]     dizindeki dosyalari listele\n");
    yaz("  gir <dizin>         calisma dizinini degistir\n");
    yaz("  geri                bir ust dizine cik\n");
    yaz("  oluştur <dosya>     boş dosya oluştur\n");
    yaz("  göster <dosya>      dosya içeriğini yaz\n");
    yaz("  defter <dosya>      metin editörünü aç\n");
    yaz("  sil <dosya>         dosya sil\n");
    yaz("  kopyala <a> <b>     dosya kopyala\n");
    yaz("  taşı <a> <b>        dosya tasi\n");
    yaz("  çalıştır <ad>       .tam uygulamasini calistir\n");
    yaz("  durum               sistem durumu\n");
    yaz("  bellek              bellek kullanimi\n");
    yaz("  süre                acilis suresi\n");
    yaz("  süreçler            etkin gorevler\n");
    yaz("  wifi-tara           kablosuz aglari tara\n");
    yaz("  wifi-baglan <ssid> [parola]\n");
    yaz("  yeniden-baslat      sistemi yeniden baslat\n");
    yaz("Ayrica dogrudan Tamga kodu yazabilirsiniz, orn:  yaz 2 + 3 * 4\n");
}

static void k_listele(int argc, char **argv, const char *cwd) {
    char dizin[KABUK_YOL_AZAMI];
    if (argc >= 2) yol_coz(cwd, argv[1], dizin, sizeof dizin);
    else           snprintf(dizin, sizeof dizin, "%s", cwd);
    char baslik[128];
    snprintf(baslik, sizeof baslik, "%s icerigi:\n", dizin);
    yaz(baslik);
    int say = 0;
    tds_listele(dizin, listele_geri, &say);
    if (say == 0) yaz("  (bos)\n");
}

static void k_durum(void) {
    char b[128];
    yaz("=== Tamga OS Durum ===\n");
    snprintf(b, sizeof b, "  Acilis suresi : %u ms\n", (unsigned)hal_zaman_ms()); yaz(b);
    snprintf(b, sizeof b, "  Bellek        : %u / %u bayt\n",
             (unsigned)bellek_kullanilan(), (unsigned)bellek_toplam()); yaz(b);
    snprintf(b, sizeof b, "  Etkin gorev   : %d\n", gorev_sayisi()); yaz(b);
    yaz("  Cekirdek      : kooperatif zamanlayici\n");
}

static void k_bellek(void) {
    char b[128];
    snprintf(b, sizeof b, "Bellek: %u kullanilan, %u bos, %u toplam (bayt)\n",
             (unsigned)bellek_kullanilan(), (unsigned)bellek_bos(),
             (unsigned)bellek_toplam());
    yaz(b);
}

static void k_sure(void) {
    char b[64];
    uint32_t ms = hal_zaman_ms();
    snprintf(b, sizeof b, "Acilistan beri: %u.%03u saniye\n", (unsigned)(ms/1000), (unsigned)(ms%1000));
    yaz(b);
}

static void k_surecler(void) {
    char b[64];
    yaz("Etkin gorevler (simulasyon):\n");
    snprintf(b, sizeof b, "  [0] kabuk        calisiyor\n"); yaz(b);
    snprintf(b, sizeof b, "  Toplam: %d gorev\n", gorev_sayisi()); yaz(b);
}

static void k_calistir(int argc, char **argv) {
    if (argc < 2) { yaz("kullanim: çalıştır <ad>\n"); return; }
    static char kaynak[KAYNAK_AZAMI];
    if (!kaynak_yukle(argv[1], kaynak, sizeof kaynak)) {
        char b[128]; snprintf(b, sizeof b, "uygulama bulunamadi: %s\n", argv[1]); yaz(b);
        return;
    }
    char hata[160];
    if (!tamga_calistir(kaynak, hata, sizeof hata)) {
        yaz(hata); yaz("\n");
    }
}

static void k_sil(int argc, char **argv, const char *cwd) {
    if (argc < 2) { yaz("kullanim: sil <dosya>\n"); return; }
    char yol[TDS_AZAMI_YOL]; yol_coz(cwd, argv[1], yol, sizeof yol);
    yaz(tds_sil(yol) == 0 ? "silindi\n" : "dosya bulunamadi\n");
}

static void k_kopyala(int argc, char **argv, const char *cwd) {
    if (argc < 3) { yaz("kullanim: kopyala <kaynak> <hedef>\n"); return; }
    char k[TDS_AZAMI_YOL], h[TDS_AZAMI_YOL];
    yol_coz(cwd, argv[1], k, sizeof k);
    yol_coz(cwd, argv[2], h, sizeof h);
    char ic[TDS_AZAMI_BOYUT];
    int n = tds_oku(k, ic, sizeof ic);
    if (n < 0) { yaz("kaynak bulunamadi\n"); return; }
    yaz(tds_yaz(h, ic, n) >= 0 ? "kopyalandi\n" : "kopyalama hatasi\n");
}

static void k_tasi(int argc, char **argv, const char *cwd) {
    if (argc < 3) { yaz("kullanim: taşı <kaynak> <hedef>\n"); return; }
    char k[TDS_AZAMI_YOL], h[TDS_AZAMI_YOL];
    yol_coz(cwd, argv[1], k, sizeof k);
    yol_coz(cwd, argv[2], h, sizeof h);
    char ic[TDS_AZAMI_BOYUT];
    int n = tds_oku(k, ic, sizeof ic);
    if (n < 0) { yaz("kaynak bulunamadi\n"); return; }
    if (tds_yaz(h, ic, n) >= 0) { tds_sil(k); yaz("tasindi\n"); }
    else yaz("tasima hatasi\n");
}

static void k_gir(int argc, char **argv, char *cwd) {
    if (argc < 2) { yaz("kullanim: gir <dizin>\n"); return; }
    if (strcmp(argv[1], "/") == 0) { strcpy(cwd, "/"); return; }
    char yeni[KABUK_YOL_AZAMI];
    yol_coz(cwd, argv[1], yeni, sizeof yeni);
    strncpy(cwd, yeni, KABUK_YOL_AZAMI - 1);
    cwd[KABUK_YOL_AZAMI - 1] = '\0';
}

static void k_geri(char *cwd) {
    char *son = strrchr(cwd, '/');
    if (!son || son == cwd) { strcpy(cwd, "/"); return; }
    *son = '\0';
}

static void k_olustur(int argc, char **argv, const char *cwd) {
    if (argc < 2) { yaz("kullanim: oluştur <dosya>\n"); return; }
    char yol[TDS_AZAMI_YOL]; yol_coz(cwd, argv[1], yol, sizeof yol);
    if (tds_var(yol)) { yaz("zaten var\n"); return; }
    yaz(tds_yaz(yol, "", 0) >= 0 ? "oluşturuldu\n" : "oluşturma hatası\n");
}

static void k_goster(int argc, char **argv, const char *cwd) {
    if (argc < 2) { yaz("kullanim: göster <dosya>\n"); return; }
    char yol[TDS_AZAMI_YOL]; yol_coz(cwd, argv[1], yol, sizeof yol);
    char ic[TDS_AZAMI_BOYUT + 1];
    int n = tds_oku(yol, ic, TDS_AZAMI_BOYUT);
    if (n < 0) { yaz("dosya bulunamadı\n"); return; }
    ic[n] = '\0';
    yaz(ic);
    if (n > 0 && ic[n - 1] != '\n') yaz("\n");
}

static void k_defter(int argc, char **argv, const char *cwd) {
    if (argc < 2) { yaz("kullanim: defter <dosya>\n"); return; }
    char yol[TDS_AZAMI_YOL]; yol_coz(cwd, argv[1], yol, sizeof yol);
    defter_calistir(yol);
}

static void k_wifi_tara(void) {
    char tampon[256];
    int n = hal_wifi_tara(tampon, sizeof tampon);
    char b[64]; snprintf(b, sizeof b, "%d ag bulundu:\n", n); yaz(b);
    yaz(tampon);
}

static void k_wifi_baglan(int argc, char **argv) {
    if (argc < 2) { yaz("kullanim: wifi-baglan <ssid> [parola]\n"); return; }
    int r = hal_wifi_baglan(argv[1], argc >= 3 ? argv[2] : "");
    yaz(r == 0 ? "baglanti basarili\n" : "baglanti basarisiz\n");
}

int komut_calistir(int argc, char **argv, char *cwd, bool *cikis) {
    const char *k = argv[0];

    if (strcmp(k, "yardım") == 0 || strcmp(k, "yardim") == 0) { k_yardim(); return 1; }
    if (strcmp(k, "listele") == 0)  { k_listele(argc, argv, cwd); return 1; }
    if (strcmp(k, "gir") == 0)      { k_gir(argc, argv, cwd); return 1; }
    if (strcmp(k, "geri") == 0)     { k_geri(cwd); return 1; }
    if (strcmp(k, "sil") == 0)      { k_sil(argc, argv, cwd); return 1; }
    if (strcmp(k, "oluştur") == 0 || strcmp(k, "olustur") == 0) { k_olustur(argc, argv, cwd); return 1; }
    if (strcmp(k, "göster") == 0 || strcmp(k, "goster") == 0)   { k_goster(argc, argv, cwd); return 1; }
    if (strcmp(k, "defter") == 0 || strcmp(k, "düzenle") == 0 || strcmp(k, "duzenle") == 0) { k_defter(argc, argv, cwd); return 1; }
    if (strcmp(k, "kopyala") == 0)  { k_kopyala(argc, argv, cwd); return 1; }
    if (strcmp(k, "taşı") == 0 || strcmp(k, "tasi") == 0) { k_tasi(argc, argv, cwd); return 1; }
    if (strcmp(k, "çalıştır") == 0 || strcmp(k, "calistir") == 0) { k_calistir(argc, argv); return 1; }
    if (strcmp(k, "durum") == 0)    { k_durum(); return 1; }
    if (strcmp(k, "bellek") == 0)   { k_bellek(); return 1; }
    if (strcmp(k, "süre") == 0 || strcmp(k, "sure") == 0) { k_sure(); return 1; }
    if (strcmp(k, "süreçler") == 0 || strcmp(k, "surecler") == 0) { k_surecler(); return 1; }
    if (strcmp(k, "wifi-tara") == 0)   { k_wifi_tara(); return 1; }
    if (strcmp(k, "wifi-baglan") == 0) { k_wifi_baglan(argc, argv); return 1; }
    if (strcmp(k, "yeniden-baslat") == 0) { yaz("yeniden baslatiliyor...\n"); *cikis = true; hal_yeniden_baslat(); return 1; }
    if (strcmp(k, "çıkış") == 0 || strcmp(k, "cikis") == 0) { *cikis = true; return 1; }

    return 0; /* yerlesik degil -> Tamga yorumlayicisina */
}
