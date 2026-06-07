/*
 * tamgaOS.ino — Tamga OS için Arduino IDE giriş noktası (ESP32).
 *
 * Arduino IDE bu .ino dosyasını ve "src/" klasörünü ÖZYİNELİ derler; bu yüzden
 * tüm Tamga çekirdeği (src/cekirdek, src/tamga, src/tds, src/kabuk) ve Arduino
 * HAL'i (src/hal/hal_arduino.cpp) otomatik derlenir. Diğer HAL'ler (host/pico/
 * esp-idf) #if guard'larıyla devre dışı kalır.
 *
 * Kurulum/flash: docs/arduino-port.md
 *   - Kart: "ESP32 Dev Module" (veya WROOM-32 kartınız)
 *   - Bu klasörü (tamgaOS) Arduino IDE'de açın, Yükle'ye basın
 *   - Seri Monitör: 115200 baud → tamga@cihaz:/$
 */
extern "C" {
  #include "src/cekirdek/cekirdek.h"
  #include "src/cekirdek/zamanlayici.h"
}

/* Tamga yorumlayıcısı özyineli çalışır; varsayılan 8 KB loopTask yığını yetmez.
   24 KB rahat bir pay verir (ESP32'de bol heap var). */
SET_LOOP_TASK_STACK_SIZE(24 * 1024);

void setup() {
  cekirdek_baslat();          // HAL + bellek + TDS + kabuk
}

void loop() {
  zamanlayici_tik();          // kooperatif: kabuk REPL bir tur (Arduino loop = ana döngü)
}
