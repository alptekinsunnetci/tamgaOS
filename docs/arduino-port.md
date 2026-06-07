# Tamga OS — Arduino IDE ile ESP32'ye Derleme

Tamga OS, **Arduino IDE** ile (ESP32 Arduino core) doğrudan derlenip yüklenebilir.
Çekirdek değişmez; Arduino'ya özgü tek dosya `src/hal/hal_arduino.cpp` ve giriş
noktası `tamgaOS.ino`'dur.

> **Durum:** Sketch ve Arduino HAL yazılmıştır. Bu makinede Arduino/ESP32 core
> kurulu değildir; `.bin` burada üretilemez. Aşağıdaki adımlar IDE'de izlenir.

## 1. Neden Arduino IDE ile çalışır?

- Arduino IDE, sketch klasöründeki `.ino` dosyasını **ve `src/` alt klasörünü
  özyineli** derler. Tamga'nın tüm kaynakları zaten `src/` altında olduğundan
  ek bir kopyalama gerekmez.
- `ARDUINO` makrosu otomatik tanımlıdır; HAL seçimi `#if defined(ARDUINO)` ile
  `hal_arduino.cpp`'ye düşer. Diğer HAL'ler (host/pico/esp-idf) guard'larla
  devre dışı kalır.
- Giriş noktası: `tamgaOS.ino` → `setup()` bir kez `cekirdek_baslat()`, `loop()`
  her turda `zamanlayici_tik()` çağırır (Arduino `loop()` zaten ana döngüdür).

## 2. Hazırlık (bir kez)

1. **Arduino IDE 2.x** kur.
2. **ESP32 core** ekle: Dosya → Tercihler → "Ek Kart Yöneticisi URL'leri":
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   sonra Kart Yöneticisi'nden **"esp32 by Espressif"** kur.

## 3. Derleme ve yükleme

1. Bu klasörü (`tamgaOS`) Arduino IDE'de aç (`tamgaOS.ino` çift tıkla).
2. **Araçlar → Kart → ESP32 Arduino → "ESP32 Dev Module"** (WROOM-32 için uygun).
3. **Araçlar → Port → COMx** (ESP32'nizin portu).
4. **Yükle (→)** düğmesine bas.
5. **Araçlar → Seri Monitör**, hız **115200** → karşılama ekranı ve
   `tamga@cihaz:/$` istemi görünür.

> İpucu: Seri Monitör'de satır sonu "Yeni Satır (NL)" veya "NL & CR" seçili olsun.

## 4. HAL eşlemesi (`hal_arduino.cpp`)

| HAL | Arduino karşılığı |
|-----|-------------------|
| `hal_baslat` | `Serial.begin(115200)` |
| `hal_zaman_ms` | `millis()` |
| `hal_bekle_ms` | `delay()` |
| `hal_uart_yaz` / `_n` | `Serial.write()` |
| `hal_uart_oku_satir` | `Serial.read()` + yankı/backspace |
| `hal_gpio_*` | `pinMode` / `digitalWrite` / `digitalRead` |
| `hal_wifi_tara` | `WiFi.scanNetworks()` — **gerçek tarama** |
| `hal_wifi_baglan` | `WiFi.begin()` — **gerçek bağlantı** |
| `hal_yeniden_baslat` | `ESP.restart()` |

WiFi, Arduino `WiFi.h` ile doğrudan çalışır:
```
tamga@cihaz:/$ wifi-tara
tamga@cihaz:/$ wifi-baglan EvAgi parola123
```
Tamga dilinden de: `wifi.tara()`, `wifi.baglan("SSID", "parola")`.

## 5. Notlar / sınırlar

- **Kalıcılık (LittleFS)**: TDS her yazmada `/tamga.img` dosyasına kaydedilir;
  yazdığın dosyalar/uygulamalar **reset sonrası kalır**. İlk açılışta örnek
  uygulamalar (`merhaba`, `bilgi`, `selam`) tohumlanır. Kendi uygulamanı
  `defter /uygulamalar/benim.tam` ile yaz, `çalıştır benim` ile çalıştır —
  kapatıp açsan da durur.
- **Bellek**: gömülü hedefte çekirdek arenası **heap'ten** ayrılır (48 KB,
  `src/tamga_yapilandirma.h`) ve tamponlar küçültülür; statik RAM ~%27, WiFi
  ile birlikte rahat sığar. Gerekirse `TAMGA_ARENA_BOYUT` ayarlanabilir.

## 5.1 Sorun giderme

- **`dram0_0_seg overflowed` / "DRAM segment data does not fit"**: statik RAM
  taşması. Tamga bunu için bellek arenasını **heap'ten** ayırır ve gömülü
  tamponları küçültür (bkz. `src/tamga_yapilandirma.h`, `TAMGA_GOMULU`). Hâlâ
  taşarsa `TAMGA_ARENA_BOYUT` / `TDS_FLASH_BOYUT` değerlerini düşürün.
- **"Multiple libraries were found for WiFi.h"**: yalnızca uyarıdır; doğru olan
  (esp32 çekirdeğindeki WiFi) kullanılır. İsterseniz
  `Documents/Arduino/libraries/WiFiNINA` klasörünü silebilirsiniz.
- **Boyut ayarı**: `src/tamga_yapilandirma.h` gömülü/host için arena, TDS dosya
  sayısı/boyutu ve flash imaj boyutunu belirler.
- **`Could not open COMx, the port is busy or doesn't exist`** ("Semafor zaman
  aşımı"): derleme başarılı, sorun **yükleme/port**. Sırayla deneyin:
  1. **Seri Monitör'ü kapatın** (en sık sebep — portu o tutuyor olabilir).
  2. **Araçlar → Port**'tan doğru COM'u seçin (ESP32'yi tak/çıkar, hangi COM
     beliriyor bakın; COM3 olmayabilir).
  3. **USB sürücüsü**: hiç port yoksa kartın çipine göre **CP2102** veya
     **CH340** sürücüsünü kurun (Aygıt Yöneticisi'nde sarı ünlem = eksik sürücü).
  4. **Veri** USB kablosu kullanın (şarj-only değil); başka USB portu deneyin.
  5. Yükleme "Connecting…" derken **BOOT (IO0)** düğmesini basılı tutun.
  6. Portu kilitleyen başka uygulamayı (PuTTY/PlatformIO) kapatın.

- **`Guru Meditation Error ... Stack canary watchpoint triggered (loopTask)`**:
  yığın taşması. Tamga yorumlayıcısı özyineli çalışır; varsayılan 8 KB loopTask
  yığını yetmez. Çözüm uygulanmıştır: `tamgaOS.ino`'da
  `SET_LOOP_TASK_STACK_SIZE(24*1024)`, gömülü hedefte küçük `Deger`/`Ortam`
  (`tamga_yapilandirma.h`) ve işlev çağrısında `Ortam`'ın heap'ten ayrılması.
  Çok derin özyinelemede hâlâ taşarsa yığını artırın veya özyinelemeyi azaltın.

## 6. Doğrulama (donanımda)

1. `çalıştır merhaba` → tanıtım çıktısı.
2. `wifi-tara` → çevredeki ağlar.
3. `defter /belgeler/not.txt` → editör; `göster /belgeler/not.txt`.
4. LED: `gpio.yon(2, 1)` + `gpio.yaz(2, 1)` (WROOM-32'de GPIO2 yerleşik LED).
