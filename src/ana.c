/* ana.c — Tamga OS giris noktasi.
 * Cekirdegi baslat, kooperatif dongu. Donanima ozgu kurulum HAL icinde
 * (hal_host.c / hal_pico.c / hal_esp32.c) yapilir.
 *
 * Giris noktasi platforma gore degisir:
 *   - Host / Pico SDK : standart main()
 *   - ESP-IDF (ESP32) : app_main() (FreeRTOS gorevi olarak cagrilir)
 *   - Arduino IDE     : giris noktasi tamgaOS.ino icindeki setup()/loop()
 */
#include "cekirdek/cekirdek.h"

#if defined(ARDUINO)
/* Arduino: main yok; tamgaOS.ino setup()'ta cekirdek_baslat(), loop()'ta
   zamanlayici_tik() cagirir. Bu dosya Arduino'da giris noktasi tanimlamaz. */

#elif defined(TAMGA_ESP32)

void app_main(void) {
    cekirdek_baslat();
    cekirdek_dongu();
}

#else

int main(void) {
    cekirdek_baslat();
    cekirdek_dongu();
    return 0;
}

#endif
