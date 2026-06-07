/* hal_esp32.c — HAL'in ESP32 (ESP32-WROOM-32, Xtensa LX6) implementasyonu.
 *
 * Yalniz ESP-IDF ile (xtensa-esp32-elf-gcc + idf.py) derlenir; host/Pico
 * yapisina dahil EDILMEZ. TAMGA_ESP32 tanimi esp32/main/CMakeLists.txt'te verilir.
 * Kurulum/flash icin: docs/esp32-port.md
 *
 * Konsol giris/cikis UART0 surucusu uzerinden yapilir (stdio ile UART0
 * surucusunun cakismasini onlemek icin dogrudan uart_* API'si kullanilir).
 */
#if defined(TAMGA_ESP32)

#include "hal.h"
#include <string.h>
#include <stdio.h>

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define KONSOL_UART   UART_NUM_0
#define KONSOL_BAUD   115200
#define RX_TAMPON     1024

/* --- Yasam dongusu --- */
void hal_baslat(void) {
    const uart_config_t cfg = {
        .baud_rate = KONSOL_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(KONSOL_UART, RX_TAMPON, 0, 0, NULL, 0);
    uart_param_config(KONSOL_UART, &cfg);
    /* UART0'in varsayilan pinleri (TX0/RX0) kullanilir; degistirmeye gerek yok. */
}

void hal_yeniden_baslat(void) {
    esp_restart();
}

/* --- Zaman --- */
uint32_t hal_zaman_ms(void) {
    return (uint32_t)(esp_timer_get_time() / 1000); /* us -> ms */
}

void hal_bekle_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/* --- Seri konsol (UART0 surucusu) --- */
void hal_uart_yaz(const char *metin) {
    uart_write_bytes(KONSOL_UART, metin, strlen(metin));
}

void hal_uart_yaz_n(const char *veri, size_t n) {
    uart_write_bytes(KONSOL_UART, veri, n);
}

int hal_uart_oku_satir(char *tampon, size_t boyut) {
    size_t i = 0;
    for (;;) {
        uint8_t c;
        int n = uart_read_bytes(KONSOL_UART, &c, 1, portMAX_DELAY);
        if (n <= 0) continue;                 /* veri bekle */
        if (c == '\r' || c == '\n') {         /* satir sonu */
            hal_uart_yaz_n("\n", 1);
            break;
        }
        if ((c == 8 || c == 127) && i > 0) {  /* backspace */
            i--; hal_uart_yaz("\b \b"); continue;
        }
        if (i < boyut - 1) {
            tampon[i++] = (char)c;
            hal_uart_yaz_n((char *)&c, 1);    /* yankila */
        }
    }
    tampon[i] = '\0';
    return (int)i;
}

/* --- GPIO --- */
void hal_gpio_yon(uint8_t pin, bool cikis) {
    gpio_reset_pin((gpio_num_t)pin);
    gpio_set_direction((gpio_num_t)pin, cikis ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
}
void hal_gpio_yaz(uint8_t pin, bool seviye) { gpio_set_level((gpio_num_t)pin, seviye); }
bool hal_gpio_oku(uint8_t pin) { return gpio_get_level((gpio_num_t)pin) != 0; }

/* --- WiFi (yer tutucu) ---
 * Tam esp_wifi STA kurulumu (esp_netif + esp_event + EventGroup) icin
 * docs/esp32-port.md'deki tarife bakin; cekirdek/REPL bunsuz de calisir. */
int hal_wifi_tara(char *tampon, size_t boyut) {
    snprintf(tampon, boyut, "(ESP32 WiFi taramasi: esp_wifi_scan ile eklenecek)\n");
    return 0;
}
int hal_wifi_baglan(const char *ssid, const char *parola) {
    (void)ssid; (void)parola;
    return -1; /* docs/esp32-port.md: esp_wifi STA baglantisi */
}

/* --- Flash blok aygiti (ileride NVS/SPIFFS/littlefs) --- */
uint32_t hal_flash_boyut(void) { return 4u * 1024u * 1024u; }
int hal_flash_oku(uint32_t adres, void *tampon, size_t boyut) {
    (void)adres; (void)tampon; (void)boyut; return -1;
}
int hal_flash_yaz(uint32_t adres, const void *veri, size_t boyut) {
    (void)adres; (void)veri; (void)boyut; return -1;
}
void hal_flash_flush(void) { /* yer tutucu: NVS/SPIFFS ile doldurulacak */ }

#endif /* TAMGA_ESP32 */
