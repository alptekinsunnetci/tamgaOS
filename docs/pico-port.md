# Tamga OS — Raspberry Pi Pico W Port Planı

Host'ta (PC) geliştirilen Tamga OS çekirdeği RP2040 / Pico W'ye **değişmeden**
derlenir; gereken tek şey HAL implementasyonu (`src/hal/hal_pico.c`) ve Pico SDK
build'idir.

> **Durum:** `hal_pico.c`, `CMakeLists.txt` ve `pico_sdk_import.cmake` hazırdır.
> Geliştirme makinesinde ARM toolchain / Pico SDK kurulu olmayabilir; o durumda
> `.uf2` burada üretilemez. Aşağıdaki adımlar toolchain kurulduktan sonra izlenir.

## 1. Gereksinimler

| Araç                | Amaç |
|---------------------|------|
| Pico SDK (2.x)      | RP2040 kütüphaneleri (`PICO_SDK_PATH`) |
| arm-none-eabi-gcc   | ARM çapraz derleyici |
| CMake (≥ 3.13)      | Yapı sistemi |
| Ninja (veya Make)   | Derleme |
| Python 3            | SDK yardımcı betikleri |

Windows'ta en kolay yol: **Raspberry Pi Pico — Windows Installer** ya da VS Code
"Raspberry Pi Pico" eklentisi (tüm araçları + ortam değişkenlerini kurar).

## 2. Yapılandırma ve derleme

```bat
set PICO_SDK_PATH=C:/pico/pico-sdk
cmake -B build-pico -G "Ninja" .
cmake --build build-pico
```
Çıktı: `build-pico/tamga.uf2` (ayrıca `.elf`, `.map`).

`CMakeLists.txt` şunları ayarlar:
- `PICO_BOARD = pico_w` (CYW43 WiFi/LED)
- `TAMGA_PICO=1` tanımı (`hal_pico.c` derlenir; host/esp32/arduino HAL'leri dışlanır)
- Seri konsol **USB-CDC** (`pico_enable_stdio_usb`)
- Kütüphaneler: `pico_stdlib`, `pico_cyw43_arch_lwip_threadsafe_background`, `hardware_gpio`

## 3. Yükleme (flash)

1. Pico W'yi **BOOTSEL** düğmesi basılıyken USB'ye tak → `RPI-RP2` sürücüsü belirir.
2. `build-pico/tamga.uf2` dosyasını bu sürücüye sürükle-bırak.
3. Pico yeniden başlar ve Tamga OS çalışır.

## 4. Konsola bağlanma

USB-CDC seri portuna bir terminalden bağlan (115200 baud; CDC'de baud önemsizdir).
Bağlanınca Tamga karşılama ekranı ve `tamga@cihaz:/$` istemi görünür.

## 5. HAL eşlemesi (`hal_pico.c`)

| HAL                  | Pico SDK karşılığı |
|----------------------|--------------------|
| `hal_baslat`         | `stdio_init_all`, `cyw43_arch_init` |
| `hal_zaman_ms`       | `to_ms_since_boot(get_absolute_time())` |
| `hal_bekle_ms`       | `sleep_ms` |
| `hal_uart_yaz`       | `fputs` (USB-CDC stdout) |
| `hal_uart_oku_satir` | `getchar_timeout_us` + yankı/backspace |
| `hal_gpio_*`         | `gpio_init/set_dir/put/get` |
| `hal_wifi_baglan`    | `cyw43_arch_wifi_connect_timeout_ms` |
| `hal_yeniden_baslat` | `watchdog_reboot` |
| `hal_flash_*`        | yer tutucu (kalıcılık ileride littlefs ile) |

## 6. Yapılacaklar

- **WiFi tarama** `hal_wifi_tara` şu an yer tutucu; `cyw43_wifi_scan` ile eklenecek.
- **Kalıcı dosya sistemi:** `hal_flash_*` + `hal_flash_flush` littlefs (Pico flash)
  ile doldurulup TDS kalıcı hale getirilecek (şu an RAM; bkz. `src/tamga_yapilandirma.h`).
- **Bellek:** gömülü hedefte arena heap'ten 48 KB; gerekirse `TAMGA_ARENA_BOYUT` ayarlanır.

## 7. Doğrulama (donanımda)

1. `çalıştır merhaba` → tanıtım çıktısı.
2. `durum` → açılış süresi, bellek, görev.
3. `defter /belgeler/not.txt` → editör; `göster` ile oku.
4. LED: `gpio.yon(0, 1)` + `gpio.yaz(0, 1)` (harici LED için bir GPIO pini;
   Pico W'de dahili LED CYW43 üzerindedir).
