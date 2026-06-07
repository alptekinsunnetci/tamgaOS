/* hal_arduino.cpp — HAL'in Arduino (ESP32 Arduino core) implementasyonu.
 *
 * Arduino IDE ile derlenir; ARDUINO makrosu otomatik tanimlidir. Bu dosya
 * yalniz o zaman derlenir; host/Pico/ESP-IDF yapilarina dahil EDILMEZ.
 * Giris noktasi: kok klasordeki tamgaOS.ino (setup/loop).
 * Kurulum/flash: docs/arduino-port.md
 *
 * Arduino C++ oldugundan, C arayuzu (hal.h) extern "C" ile saglanir.
 * WiFi burada Arduino WiFi kutuphanesiyle GERCEKTEN calisir.
 */
#if defined(ARDUINO)

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <stdio.h>
#include <string.h>

extern "C" {
#include "hal.h"
}

/* --- Kalici flash imaji (LittleFS dosyasi /tamga.img ile yedekli) --- */
#include "../tamga_yapilandirma.h"
#define ARD_FLASH_BOYUT TDS_FLASH_BOYUT
#define ARD_FLASH_DOSYA "/tamga.img"
static uint8_t s_img[ARD_FLASH_BOYUT];

static void flash_yukle(void) {
    if (!LittleFS.exists(ARD_FLASH_DOSYA)) return;
    File f = LittleFS.open(ARD_FLASH_DOSYA, "r");
    if (!f) return;
    f.read(s_img, ARD_FLASH_BOYUT);
    f.close();
}

/* --- Yasam dongusu --- */
extern "C" void hal_baslat(void) {
    Serial.begin(115200);
    unsigned long t = millis();
    while (!Serial && (millis() - t) < 2000) { /* USB-CDC kartlar icin bekle */ }
    LittleFS.begin(true);   /* gerekirse bicimlendir */
    flash_yukle();
}

extern "C" void hal_yeniden_baslat(void) {
    ESP.restart();
}

/* --- Zaman --- */
extern "C" uint32_t hal_zaman_ms(void) { return (uint32_t)millis(); }
extern "C" void     hal_bekle_ms(uint32_t ms) { delay(ms); }

/* --- Seri konsol --- */
extern "C" void hal_uart_yaz(const char *metin) {
    Serial.write((const uint8_t *)metin, strlen(metin));
}
extern "C" void hal_uart_yaz_n(const char *veri, size_t n) {
    Serial.write((const uint8_t *)veri, n);
}
extern "C" int hal_uart_oku_satir(char *tampon, size_t boyut) {
    size_t i = 0;
    for (;;) {
        while (Serial.available() <= 0) { delay(1); }
        int c = Serial.read();
        if (c < 0) continue;
        if (c == '\r' || c == '\n') { Serial.write('\n'); break; }
        if ((c == 8 || c == 127) && i > 0) { i--; Serial.print("\b \b"); continue; }
        if (i < boyut - 1) { tampon[i++] = (char)c; Serial.write((uint8_t)c); }
    }
    tampon[i] = '\0';
    return (int)i;
}

/* --- GPIO --- */
extern "C" void hal_gpio_yon(uint8_t pin, bool cikis) { pinMode(pin, cikis ? OUTPUT : INPUT); }
extern "C" void hal_gpio_yaz(uint8_t pin, bool seviye) { digitalWrite(pin, seviye ? HIGH : LOW); }
extern "C" bool hal_gpio_oku(uint8_t pin) { return digitalRead(pin) == HIGH; }

/* --- WiFi (Arduino WiFi kutuphanesi ile gercek) --- */
extern "C" int hal_wifi_tara(char *tampon, size_t boyut) {
    int n = WiFi.scanNetworks();
    size_t p = 0;
    if (n <= 0) { snprintf(tampon, boyut, "(ağ bulunamadı)\n"); WiFi.scanDelete(); return 0; }
    for (int i = 0; i < n && p < boyut; i++) {
        p += snprintf(tampon + p, boyut - p, "%s (%d dBm)\n",
                      WiFi.SSID(i).c_str(), (int)WiFi.RSSI(i));
    }
    WiFi.scanDelete();
    return n;
}
extern "C" int hal_wifi_baglan(const char *ssid, const char *parola) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, parola);
    unsigned long bas = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - bas) < 15000) delay(200);
    return WiFi.status() == WL_CONNECTED ? 0 : -1;
}

/* --- Flash blok aygiti (KALICI, LittleFS destekli) --- */
extern "C" uint32_t hal_flash_boyut(void) { return ARD_FLASH_BOYUT; }
extern "C" int hal_flash_oku(uint32_t adres, void *tampon, size_t boyut) {
    if (adres + boyut > ARD_FLASH_BOYUT) return -1;
    memcpy(tampon, s_img + adres, boyut);
    return 0;
}
extern "C" int hal_flash_yaz(uint32_t adres, const void *veri, size_t boyut) {
    if (adres + boyut > ARD_FLASH_BOYUT) return -1;
    memcpy(s_img + adres, veri, boyut);
    return 0;
}
extern "C" void hal_flash_flush(void) {
    File f = LittleFS.open(ARD_FLASH_DOSYA, "w");
    if (!f) return;
    f.write(s_img, ARD_FLASH_BOYUT);
    f.close();
}

#endif /* ARDUINO */
