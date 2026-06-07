/* hal.h — Tamga OS Donanim Soyutlama Katmani (HAL)
 *
 * Donanima bagli HER SEY bu arayuzun arkasinda durur. Ust katmanlar
 * (cekirdek, tds, tamga, kabuk) yalnizca bu basligi cagirir; boylece ayni
 * kod hem PC'de (hal_host.c) hem Pico W'de (hal_pico.c) calisir.
 */
#ifndef TAMGA_HAL_H
#define TAMGA_HAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* --- Yasam dongusu --- */
void     hal_baslat(void);          /* donanimi/stdio'yu hazirla */
void     hal_yeniden_baslat(void);  /* sistemi yeniden baslat */

/* --- Zaman --- */
uint32_t hal_zaman_ms(void);        /* acilistan beri gecen milisaniye */
void     hal_bekle_ms(uint32_t ms); /* mesgul/uyku bekleme */

/* --- Seri konsol (UART / USB-stdio) --- */
void     hal_uart_yaz(const char *metin);         /* sonlandirilmis dizgi yaz */
void     hal_uart_yaz_n(const char *veri, size_t n);
int      hal_uart_oku_satir(char *tampon, size_t boyut); /* bir satir oku; uzunluk veya -1 */

/* --- GPIO (Milestone 1: host'ta stub) --- */
void     hal_gpio_yon(uint8_t pin, bool cikis);
void     hal_gpio_yaz(uint8_t pin, bool seviye);
bool     hal_gpio_oku(uint8_t pin);

/* --- WiFi (Milestone 1: stub) --- */
int      hal_wifi_tara(char *tampon, size_t boyut); /* bulunan ag sayisi */
int      hal_wifi_baglan(const char *ssid, const char *parola);

/* --- Flash blok aygiti (TDS kaliciligi icin) ---
 * Byte-adresli okuma/yazma + flush. Yaz cagrilari bir RAM aynaya yazabilir;
 * hal_flash_flush() degisiklikleri kalici depoya isler (host: dosya,
 * Arduino: LittleFS). Stub platformlarda flush bos gecer (kalicilik yok). */
int      hal_flash_oku(uint32_t adres, void *tampon, size_t boyut);
int      hal_flash_yaz(uint32_t adres, const void *veri, size_t boyut);
void     hal_flash_flush(void);
uint32_t hal_flash_boyut(void);

#endif /* TAMGA_HAL_H */
