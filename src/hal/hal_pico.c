/* hal_pico.c — HAL'in Raspberry Pi Pico W (RP2040) implementasyonu.
 *
 * Bu dosya yalnizca Pico SDK ile (arm-none-eabi-gcc + CMake) derlenir; host
 * yapisina (MinGW) dahil EDILMEZ. TAMGA_PICO tanimi CMakeLists.txt'te verilir.
 * Bkz. docs/pico-port.md — toolchain kurulumu ve flash adimlari.
 */
#if defined(TAMGA_PICO)

#include "hal.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include <string.h>
#include <stdio.h>

static bool s_wifi_hazir = false;

void hal_baslat(void) {
    stdio_init_all();          /* USB-CDC ve/veya UART uzerinden seri konsol */
    /* CYW43 (WiFi + dahili LED) baslat; basarisiz olsa da sistem calisir. */
    if (cyw43_arch_init() == 0) s_wifi_hazir = true;
}

void hal_yeniden_baslat(void) {
    /* Watchdog ile yeniden baslat */
    watchdog_reboot(0, 0, 0);
    for (;;) tight_loop_contents();
}

uint32_t hal_zaman_ms(void) {
    return to_ms_since_boot(get_absolute_time());
}

void hal_bekle_ms(uint32_t ms) {
    sleep_ms(ms);
}

void hal_uart_yaz(const char *metin) {
    fputs(metin, stdout);
}

void hal_uart_yaz_n(const char *veri, size_t n) {
    for (size_t i = 0; i < n; i++) putchar(veri[i]);
}

int hal_uart_oku_satir(char *tampon, size_t boyut) {
    size_t i = 0;
    for (;;) {
        int c = getchar_timeout_us(0xFFFFFFFF); /* blokla */
        if (c == PICO_ERROR_TIMEOUT) continue;
        if (c == '\r' || c == '\n') { putchar('\n'); break; }
        if ((c == 8 || c == 127) && i > 0) {     /* backspace */
            i--; fputs("\b \b", stdout); continue;
        }
        if (i < boyut - 1) { tampon[i++] = (char)c; putchar(c); /* yankila */ }
    }
    tampon[i] = '\0';
    return (int)i;
}

void hal_gpio_yon(uint8_t pin, bool cikis) {
    gpio_init(pin);
    gpio_set_dir(pin, cikis ? GPIO_OUT : GPIO_IN);
}
void hal_gpio_yaz(uint8_t pin, bool seviye) { gpio_put(pin, seviye); }
bool hal_gpio_oku(uint8_t pin) { return gpio_get(pin); }

int hal_wifi_tara(char *tampon, size_t boyut) {
    /* Basit yer tutucu: gercek tarama cyw43_wifi_scan ile asenkron yapilir;
       ileri milestone'da doldurulacak. */
    if (!s_wifi_hazir) { snprintf(tampon, boyut, "(WiFi hazir degil)\n"); return 0; }
    snprintf(tampon, boyut, "(tarama ileri surumde)\n");
    return 0;
}

int hal_wifi_baglan(const char *ssid, const char *parola) {
    if (!s_wifi_hazir) return -1;
    cyw43_arch_enable_sta_mode();
    return cyw43_arch_wifi_connect_timeout_ms(
        ssid, parola, CYW43_AUTH_WPA2_AES_PSK, 30000);
}

/* Flash blok aygiti — ileride littlefs icin; simdilik yer tutucu (kalicilik yok). */
uint32_t hal_flash_boyut(void) { return 2u * 1024u * 1024u; }
int hal_flash_oku(uint32_t adres, void *tampon, size_t boyut) {
    (void)adres; (void)tampon; (void)boyut; return -1;
}
int hal_flash_yaz(uint32_t adres, const void *veri, size_t boyut) {
    (void)adres; (void)veri; (void)boyut; return -1;
}
void hal_flash_flush(void) { /* yer tutucu: littlefs ile doldurulacak */ }

#endif /* TAMGA_PICO */
