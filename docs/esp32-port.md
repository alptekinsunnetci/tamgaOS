# Tamga OS — ESP32 (ESP32-WROOM-32) Port Planı

Tamga OS'un taşınabilir çekirdeği **değişmeden** ESP32 üzerinde çalışır; gereken
tek şey bir HAL implementasyonu (`src/hal/hal_esp32.c`) ve ESP-IDF build'idir.

> **Durum:** `hal_esp32.c` ve `esp32/` ESP-IDF projesi yazılmıştır. Bu geliştirme
> makinesinde ESP-IDF/xtensa toolchain **kurulu değildir**; bu yüzden `.bin`
> burada üretilemez. Aşağıdaki adımlar toolchain kurulduktan sonra izlenir.

## 1. Neden çalışır?

| Katman | Durum |
|--------|-------|
| Çekirdek (lexer/ayrıştırıcı/yorumlayıcı/TDS/kabuk/defter/diziler) | **Saf C11 — değişmez** |
| Donanım erişimi | Tamamı `hal.h` arkasında; `hal_esp32.c` ile sağlanır |
| Giriş noktası | `ana.c` içinde `TAMGA_ESP32` ile `app_main()` seçilir |
| Bellek | 96 KB arena, ESP32'nin ~520 KB RAM'ine rahat sığar |

RP2040'a özgü hiçbir kod yoktur; platform farkları yalnızca HAL + toolchain'dir.

## 2. Gereksinimler

| Araç | Amaç |
|------|------|
| ESP-IDF (v5.x) | ESP32 SDK + `idf.py` |
| xtensa-esp32-elf-gcc | ESP32 çapraz derleyici (ESP-IDF kurar) |
| Python 3 | ESP-IDF araçları |

Kurulum: [ESP-IDF Get Started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
(Windows'ta "ESP-IDF Tools Installer" veya VS Code "Espressif IDF" eklentisi).

## 3. Derleme ve flash

```bat
cd esp32
idf.py set-target esp32
idf.py build
idf.py -p COM3 flash monitor       :: kendi seri portunuzu yazın
```
`esp32/CMakeLists.txt` projeyi, `esp32/main/CMakeLists.txt` ise kaynakları
(repo kökündeki `../../src` altından) bir ESP-IDF bileşeni olarak kaydeder ve
`TAMGA_ESP32=1` tanımıyla `hal_esp32.c`'yi seçer.

## 4. Konsola bağlanma

`idf.py monitor` (115200 baud, UART0). Bağlanınca Tamga karşılama ekranı ve
`tamga@cihaz:/$` istemi görünür. Çıkmak için `Ctrl+]`.

## 5. HAL eşlemesi (`hal_esp32.c`)

| HAL | ESP-IDF karşılığı |
|-----|-------------------|
| `hal_baslat` | `uart_driver_install` + `uart_param_config` (UART0) |
| `hal_zaman_ms` | `esp_timer_get_time() / 1000` |
| `hal_bekle_ms` | `vTaskDelay(pdMS_TO_TICKS(...))` |
| `hal_uart_yaz` / `_n` | `uart_write_bytes` |
| `hal_uart_oku_satir` | `uart_read_bytes` + yankı/backspace |
| `hal_gpio_*` | `driver/gpio.h` (`gpio_set_direction/level`) |
| `hal_yeniden_baslat` | `esp_restart` |

> Konsol I/O doğrudan UART0 sürücüsüyle yapılır (stdio/`printf` ile sürücü
> çakışmasını önlemek için). İsteğe bağlı: USB-CDC olan ESP32-S2/S3'te
> `tinyusb`/`esp_vfs_usb_serial_jtag` ile değiştirilebilir.

## 6. Yapılacaklar (port tamamlama)

- **WiFi**: `hal_wifi_tara`/`hal_wifi_baglan` şu an yer tutucu. Tam STA için:
  `nvs_flash_init` → `esp_netif_init` → `esp_event_loop_create_default` →
  `esp_wifi_init` → STA config → `esp_wifi_start`/`esp_wifi_connect`,
  bağlantı için bir `EventGroup` biti. Tarama: `esp_wifi_scan_start` +
  `esp_wifi_scan_get_ap_records`.
- **Kalıcı dosya sistemi**: TDS şu an RAM. ESP32'de NVS veya SPIFFS/LittleFS
  ile `hal_flash_*` doldurulup TDS kalıcı hale getirilebilir.
- **Görev gözcüsü (WDT)**: kabuk döngüsü çoğunlukla `uart_read_bytes` içinde
  bloklanır (CPU'yu bırakır). Uzun süren `.tam` döngülerinde gerekirse
  `vTaskDelay(1)` ya da `esp_task_wdt` beslemesi eklenir.

## 7. Doğrulama (donanımda)

1. `çalıştır merhaba` → tanıtım çıktısı.
2. `durum` → açılış süresi, bellek, görev.
3. `defter /belgeler/not.txt` → editör; `göster` ile oku.
4. LED: `gpio.yon(2, 1)` + `gpio.yaz(2, 1)` (WROOM-32'de GPIO2 yerleşik LED).
