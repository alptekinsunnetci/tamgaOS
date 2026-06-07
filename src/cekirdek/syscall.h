/* syscall.h — Tamga sistem cagri yuzeyi.
 * Tamga dili moduller bu ince katman uzerinden cekirdek/HAL kaynaklarina
 * erisir. Boylece dil runtime'i dogrudan HAL'e degil tek bir kapiya baglanir.
 */
#ifndef TAMGA_SYSCALL_H
#define TAMGA_SYSCALL_H

#include <stddef.h>
#include <stdint.h>

/* Konsol */
void     sys_yaz(const char *metin);
int      sys_oku_satir(char *tampon, size_t boyut);  /* bir satir oku; uzunluk veya -1 */

/* Zaman */
uint32_t sys_zaman_ms(void);
void     sys_bekle_ms(uint32_t ms);

/* Bellek bilgisi */
size_t   sys_bellek_toplam(void);
size_t   sys_bellek_kullanilan(void);

/* GPIO */
void     sys_gpio_yon(uint8_t pin, int cikis);
void     sys_gpio_yaz(uint8_t pin, int seviye);
int      sys_gpio_oku(uint8_t pin);

/* WiFi */
int      sys_wifi_tara(char *tampon, size_t boyut);
int      sys_wifi_baglan(const char *ssid, const char *parola);

#endif /* TAMGA_SYSCALL_H */
