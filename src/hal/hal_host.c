/* hal_host.c — HAL'in PC (MinGW gcc) implementasyonu.
 * Seri konsol = stdio, zaman = clock(), flash = RAM tampon.
 * Bu makinede derlenip test edilen dosyadir.
 * Yalniz host icin: gomulu hedefler (Pico/ESP32/Arduino) kendi HAL'ini kullanir.
 */
#if !defined(TAMGA_PICO) && !defined(TAMGA_ESP32) && !defined(ARDUINO)

#include "hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

static void flash_yukle(void);  /* asagida tanimli */

/* --- Yasam dongusu --- */
void hal_baslat(void) {
#ifdef _WIN32
    /* Windows konsolunda UTF-8 cikti icin kod sayfasini ayarla */
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    setvbuf(stdout, NULL, _IONBF, 0);
    flash_yukle();   /* kalici flash imajini diskten yukle */
}

void hal_yeniden_baslat(void) {
    hal_uart_yaz("[host] yeniden baslatma simulasyonu — cikiliyor.\n");
    exit(0);
}

/* --- Zaman --- */
static uint32_t s_baslangic_ms = 0;

static uint32_t simdi_ms(void) {
#ifdef _WIN32
    return (uint32_t)GetTickCount();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000u + ts.tv_nsec / 1000000u);
#endif
}

uint32_t hal_zaman_ms(void) {
    uint32_t s = simdi_ms();
    if (s_baslangic_ms == 0) s_baslangic_ms = s;
    return s - s_baslangic_ms;
}

void hal_bekle_ms(uint32_t ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec ts = { ms / 1000, (long)(ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
#endif
}

/* --- Seri konsol --- */
void hal_uart_yaz(const char *metin) {
    fputs(metin, stdout);
}

void hal_uart_yaz_n(const char *veri, size_t n) {
    fwrite(veri, 1, n, stdout);
}

int hal_uart_oku_satir(char *tampon, size_t boyut) {
    if (!fgets(tampon, (int)boyut, stdin)) return -1;
    size_t u = strlen(tampon);
    while (u > 0 && (tampon[u - 1] == '\n' || tampon[u - 1] == '\r'))
        tampon[--u] = '\0';
    return (int)u;
}

/* --- GPIO (stub) --- */
static bool s_pin[40];
void hal_gpio_yon(uint8_t pin, bool cikis) { (void)pin; (void)cikis; }
void hal_gpio_yaz(uint8_t pin, bool seviye) { if (pin < 40) s_pin[pin] = seviye; }
bool hal_gpio_oku(uint8_t pin) { return pin < 40 ? s_pin[pin] : false; }

/* --- WiFi (stub) --- */
int hal_wifi_tara(char *tampon, size_t boyut) {
    snprintf(tampon, boyut, "EvAgi\nKomsuWiFi\nTamgaTest\n");
    return 3;
}
int hal_wifi_baglan(const char *ssid, const char *parola) {
    (void)parola;
    char ileti[128];
    snprintf(ileti, sizeof ileti, "[host] '%s' agina baglanma simule edildi.\n", ssid);
    hal_uart_yaz(ileti);
    return 0;
}

/* --- Flash blok aygiti (dosya-destekli, KALICI) ---
 * RAM ayna 'tamga_flash.bin' dosyasiyla yedeklenir: acilista yuklenir,
 * hal_flash_flush() ile diske yazilir. Boylece kalicilik PC'de test edilir. */
#include "../tamga_yapilandirma.h"
#define HOST_FLASH_BOYUT TDS_FLASH_BOYUT
#define HOST_FLASH_DOSYA "tamga_flash.bin"
static uint8_t s_flash[HOST_FLASH_BOYUT];

static void flash_yukle(void) {
    FILE *f = fopen(HOST_FLASH_DOSYA, "rb");
    if (!f) return;                 /* ilk acilis: dosya yok */
    fread(s_flash, 1, HOST_FLASH_BOYUT, f);
    fclose(f);
}

int hal_flash_oku(uint32_t adres, void *tampon, size_t boyut) {
    if (adres + boyut > HOST_FLASH_BOYUT) return -1;
    memcpy(tampon, s_flash + adres, boyut);
    return 0;
}
int hal_flash_yaz(uint32_t adres, const void *veri, size_t boyut) {
    if (adres + boyut > HOST_FLASH_BOYUT) return -1;
    memcpy(s_flash + adres, veri, boyut);
    return 0;
}
void hal_flash_flush(void) {
    FILE *f = fopen(HOST_FLASH_DOSYA, "wb");
    if (!f) return;
    fwrite(s_flash, 1, HOST_FLASH_BOYUT, f);
    fclose(f);
}
uint32_t hal_flash_boyut(void) { return HOST_FLASH_BOYUT; }

#endif /* !TAMGA_PICO */
