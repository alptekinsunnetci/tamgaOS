/* syscall.c — sistem cagrilarinin HAL/cekirdek'e baglanmasi. */
#include "syscall.h"
#include "bellek.h"
#include "../hal/hal.h"

void     sys_yaz(const char *metin)        { hal_uart_yaz(metin); }
int      sys_oku_satir(char *t, size_t b)  { return hal_uart_oku_satir(t, b); }
uint32_t sys_zaman_ms(void)                { return hal_zaman_ms(); }
void     sys_bekle_ms(uint32_t ms)         { hal_bekle_ms(ms); }
size_t   sys_bellek_toplam(void)           { return bellek_toplam(); }
size_t   sys_bellek_kullanilan(void)       { return bellek_kullanilan(); }
void     sys_gpio_yon(uint8_t pin, int c)  { hal_gpio_yon(pin, c != 0); }
void     sys_gpio_yaz(uint8_t pin, int s)  { hal_gpio_yaz(pin, s != 0); }
int      sys_gpio_oku(uint8_t pin)         { return hal_gpio_oku(pin) ? 1 : 0; }
int      sys_wifi_tara(char *t, size_t b)  { return hal_wifi_tara(t, b); }
int      sys_wifi_baglan(const char *s, const char *p) { return hal_wifi_baglan(s, p); }
