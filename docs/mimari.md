# Tamga OS — Mimari

Tamga OS, RP2040 (Raspberry Pi Pico W) için tasarlanmış, **tamamen Türkçe**,
Unix felsefesinden ilham alan minimal bir işletim sistemidir. Hiçbir kullanıcı
uygulaması C binary olarak çalışmaz; tüm uygulamalar `.tam` dosyaları olarak
**Tamga Dili** ile yazılır ve yorumlanır.

## Katmanlar

```
        +-------------------------------------------------+
   K    |  KABUK (kabuk/)                                  |
   u    |  Türkçe REPL · yardım/listele/çalıştır/durum ...  |
   l    +-------------------------------------------------+
   l    |  TAMGA DİLİ (tamga/)                             |
   a    |  tarayıcı -> ayrıştırıcı -> yorumlayıcı          |
   n    |  moduller: sistem · dosya · zaman · gpio · wifi  |
   ı    +-------------------------------------------------+
   c    |  TDS — Tamga Dosya Sistemi (tds/)                |
   ı    |  oku · yaz · sil · listele (RAM; ileride flash)  |
        +-------------------------------------------------+
        |  ÇEKİRDEK (cekirdek/)                            |
        |  zamanlayıcı · bellek · olay · syscall           |
        +-------------------------------------------------+
        |  HAL (hal/hal.h)                                |
        +-------------------+-------------------+----------------+
        | hal_host.c (PC)   | hal_pico.c (RP2040)| hal_esp32.c   |
        +-------------------+-------------------+----------------+
        | stdio · saat · RAM| USB-CDC · timer ·  | UART0 · esp_  |
        |                   | GPIO · CYW43 WiFi  | timer · GPIO  |
        |                   | · flash            | · esp_wifi    |
        +-------------------+-------------------+----------------+

Platform seçimi derleme bayrağıyla: host (bayrak yok), `TAMGA_PICO`, `TAMGA_ESP32`.
```

Donanıma bağlı **her şey** `hal.h` arkasındadır. Üst katmanlar yalnızca HAL'i
çağırır; böylece aynı çekirdek hem PC'de hem Pico W'de derlenir.

## Veri akışı: bir satırın işlenişi

```
"yaz 2 + 3 * 4"
      |
      v
  tarayıcı  -> [YAZ][SAYI 2][ARTI][SAYI 3][CARPI][SAYI 4][DOSYASONU]
      |
      v
 ayrıştırıcı -> D_YAZ
                  └─ D_IKILI(+)
                       ├─ D_SAYI 2
                       └─ D_IKILI(*)
                            ├─ D_SAYI 3
                            └─ D_SAYI 4
      |
      v
 yorumlayıcı -> ağacı yürüt -> 14 -> hal_uart_yaz("14\n")
```

## Çekirdek bileşenleri

| Bileşen        | Dosya                      | Görev |
|----------------|----------------------------|-------|
| Bellek         | `cekirdek/bellek.c`        | Sabit 96 KB arena üzerinde serbest-listeli ayırıcı (malloc yok) |
| Zamanlayıcı    | `cekirdek/zamanlayici.c`   | Kooperatif round-robin görev tablosu |
| Olay           | `cekirdek/olay.c`          | Halka tampon mesaj kuyruğu |
| Syscall        | `cekirdek/syscall.c`       | Modüllerin HAL/çekirdeğe eriştiği ince kapı |
| Çekirdek       | `cekirdek/cekirdek.c`      | Alt sistemleri başlatır, ana döngü |

## Tasarım ilkeleri

- **Türkçe-öncelikli**: anahtar kelimeler, komutlar, hata mesajları Türkçe.
- **Gömülü-öncelikli**: sabit arenalar, öngörülebilir bellek, küçük yığın izi.
- **Unix esinli**: küçük, birleşebilen parçalar (tarayıcı → ayrıştırıcı → yorumlayıcı).
- **Taşınabilir çekirdek + ince HAL**: donanım bağımlılığı tek sınırda toplanır.

## Bellek bütçesi (RP2040, 264 KB RAM)

- Çekirdek arena: 96 KB (`TAMGA_ARENA_BOYUT`, AST + dinamik veri)
- Ortam çerçevesi (işlev çağrısı): ~6 KB yığın (özyineleme derinliğini sınırlar)
- TDS RAM: `TDS_AZAMI_DOSYA × TDS_AZAMI_BOYUT` (öntanımlı 32 × 2 KB)

Bu bütçeler `cekirdek/bellek.c`, `tamga/ortam.h`, `tds/tds.h` başında ayarlanır.

## Sonraki milestone'lar

1. ✅ **Kalıcılık**: TDS, `hal_flash` blok aygıtı üzerine serileştirilir;
   her yazmada otomatik kaydedilir. Host'ta `tamga_flash.bin`, Arduino'da
   LittleFS (`/tamga.img`). Pico/ESP-IDF flash sürücüsü sonraki adım.
2. ✅ **Diziler** (`[...]`, indeksleme, `uzunluk/ekle/çıkar/dizi`) — tamamlandı.
3. Gerçek WiFi (ESP-IDF tarafı): `esp_wifi_scan` (Arduino'da zaten gerçek).
4. ✅ **Sözlükler** (`{...}`, `s["k"]`, `anahtarlar/var_mı/sil_anahtar/sözlük`) — tamamlandı.
5. Çoklu görev: kabuk + arka plan `.tam` uygulamaları aynı zamanlayıcıda.
