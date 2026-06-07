# Tamga OS — Raspberry Pi Pico W Port Planı

Bu belge, host'ta (PC) geliştirilen Tamga OS çekirdeğinin RP2040 / Pico W'ye
nasıl derlenip yükleneceğini anlatır.

> **Durum:** Çekirdek taşınabilir ve `hal_pico.c` yazılmıştır. Ancak bu
> geliştirme makinesinde **ARM toolchain / Pico SDK / CMake / Python kurulu
> değildir**; bu yüzden `.uf2` burada üretilemez. Aşağıdaki adımlar, toolchain
> kurulduktan sonra izlenir.

## 1. Gereksinimler

| Araç                    | Amaç |
|-------------------------|------|
| Pico SDK (2.x)          | RP2040 kütüphaneleri (`PICO_SDK_PATH`) |
| arm-none-eabi-gcc       | ARM çapraz derleyici |
| CMake (≥ 3.13)          | Yapı sistemi |
| Ninja (veya Make)       | Derleme |
| Python 3                | SDK yardımcı betikleri |

Windows'ta en kolay yol: **Raspberry Pi Pico — Windows Installer** (tüm
araçları + ortam değişkenlerini kurar) ya da VS Code "Raspberry Pi Pico" eklentisi.

## 2. Yapılandırma ve derleme

```bat
set PICO_SDK_PATH=C:/pico/pico-sdk
cmake -B build-pico -G "Ninja" .
cmake --build build-pico
```
Çıktı: `build-pico/tamga.uf2` (ayrıca `.elf`, `.map`).

`CMakeLists.txt` şunları ayarlar:
- `PICO_BOARD = pico_w` (CYW43 WiFi/LED için)
- `TAMGA_PICO=1` tanımı (`hal_pico.c` bu tanımla derlenir, `hal_host.c` dışlanır)
- Seri konsol **USB-CDC** üzerinden (`pico_enable_stdio_usb`)
- Bağlanan kütüphaneler: `pico_stdlib`, `pico_cyw43_arch_lwip_threadsafe_background`, `hardware_gpio`

## 3. Yükleme (flash)

1. Pico W'yi **BOOTSEL** düğmesi basılıyken USB'ye tak → `RPI-RP2` sürücüsü belirir.
2. `build-pico/tamga.uf2` dosyasını bu sürücüye sürükle-bırak.
3. Pico yeniden başlar ve Tamga OS çalışır.

## 4. Konsola bağlanma

USB-CDC seri portuna bir terminalden bağlan (115200 baud; CDC'de baud önemsizdir):
- Windows: PuTTY / `mode` ile COM portu, ya da VS Code seri monitör.
- Bağlanınca Tamga karşılama ekranı ve `tamga@cihaz:/$` istemi görünür.

## 5. HAL eşlemesi (`hal_pico.c`)

| HAL                     | Pico SDK karşılığı |
|-------------------------|--------------------|
| `hal_baslat`            | `stdio_init_all`, `cyw43_arch_init` |
| `hal_zaman_ms`          | `to_ms_since_boot(get_absolute_time())` |
| `hal_bekle_ms`          | `sleep_ms` |
| `hal_uart_yaz`          | `fputs` (USB-CDC stdout) |
| `hal_uart_oku_satir`    | `getchar_timeout_us` + yankı/backspace |
| `hal_gpio_*`            | `gpio_init/set_dir/put/get` |
| `hal_wifi_baglan`       | `cyw43_arch_wifi_connect_timeout_ms` |
| `hal_yeniden_baslat`    | `watchdog_reboot` |

## 6. Bilinen sınırlar / yapılacaklar

- **WiFi tarama** `hal_wifi_tara` şu an yer tutucu; `cyw43_wifi_scan` ile
  asenkron tarama eklenecek.
- **Kalıcı dosya sistemi**: TDS şu an RAM. Pico flash'ta littlefs için
  `hal_flash_*` doldurulup `tds` katmanı littlefs üzerine taşınacak.
- **Yığın boyutu**: işlev çağrısı çerçevesi (`Ortam`) ~6 KB. Derin özyineleme
  264 KB RAM'de yığını taşırabilir; gerekirse `ORTAM_AZAMI` küçültülebilir.
- **Bellek arenası**: `TAMGA_ARENA_BOYUT` (96 KB) Pico için uygundur; ihtiyaca
  göre `cekirdek/bellek.c` başından ayarlanır.

## 7. Doğrulama (donanımda)

1. `çalıştır merhaba` → tanıtım çıktısı.
2. `durum` → açılış süresi, bellek, görev sayısı.
3. `yaz 2 + 3 * 4` → `14`.
4. LED testi: `gpio.yon(0, 1)` ardından `gpio.yaz(0, 1)` (Pico W'de dahili LED
   CYW43 üzerindedir; harici LED için bir GPIO pini kullanın).
