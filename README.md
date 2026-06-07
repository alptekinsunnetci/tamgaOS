<<<<<<< HEAD
# tamgaOS
tamgaOS
=======
# Tamga OS

**Tamga OS**, Raspberry Pi Pico W (RP2040) için tasarlanmış, **tamamen Türkçe**,
Unix felsefesinden ilham alan minimal bir işletim sistemidir. Hiçbir kullanıcı
uygulaması C binary olarak çalışmaz; tüm uygulamalar `.tam` dosyaları olarak
**Tamga Dili** ile yazılır ve yorumlanır.

```
==========================================
   TAMGA OS — Türkçe İşletim Sistemi
   Sürüm 0.1 (dikey dilim)
==========================================
tamga@cihaz:/$ çalıştır merhaba
Merhaba Dünya
Büyük
0
1
2
3
4
toplam = 30
tamga@cihaz:/$ yaz 2 + 3 * 4
14
```

## Katmanlar

1. **Çekirdek** — kooperatif zamanlayıcı, arena bellek, olay kuyruğu, syscall.
2. **TDS** — Tamga Dosya Sistemi (oku/yaz/sil/listele).
3. **Kabuk** — Türkçe REPL ve komutlar.
4. **Tamga Dili** — tarayıcı → ayrıştırıcı → ağaç-yürüyen yorumlayıcı + modüller.

Donanım bağımlılığı tek bir sınırda (`src/hal/hal.h`) toplanır; bu sayede aynı
çekirdek **PC** (geliştirme), **Raspberry Pi Pico W** (RP2040) ve
**ESP32** (ESP32-WROOM-32) üzerinde değişmeden derlenir — her platform için
yalnızca bir HAL dosyası (`hal_host.c` / `hal_pico.c` / `hal_esp32.c`).

## Hızlı başlangıç (PC / Windows + MinGW)

```bat
mingw32-make            :: build\tamga.exe üretir
mingw32-make test       :: tüm testler (lexer 11/11 + dil 25/25)
mingw32-make lextest    :: yalnız lexer testleri
mingw32-make diltest    :: yalnız dil (ayrıştırıcı + yorumlayıcı) testleri
mingw32-make calistir   :: OS'u başlat (REPL)
```

Örnek uygulamalar `uygulamalar/` altında: `merhaba`, `hesapmakinesi`,
`bilgi` (sistem/dosya modülleri), `led` (gpio/zaman), `diziler` (dizi işlemleri),
`sozlukler` (sözlük işlemleri), `dongu` (her/for-each), `metinler` (metin işlevleri),
`selamla` (etkileşimli giriş). Açılışta bazıları TDS'e tohumlanır
(`çalıştır <ad>` ve `listele /uygulamalar` hem PC'de hem Pico'da çalışır).

## Unix-benzeri kabuk

Dosya komutları: `listele` (ls), `gir`/`geri` (cd), `oluştur` (touch),
`göster` (cat), `defter` (metin editörü), `sil` (rm), `kopyala` (cp),
`taşı` (mv). Sistem: `durum`, `bellek`, `süre`, `süreçler`, `wifi-tara`.

**Defter** — satır tabanlı metin editörü (`ed`/`nano` ruhunda, seri konsolda çalışır):
```
tamga@cihaz:/$ defter /belgeler/notlar.txt
defter> ekle           # ekleme modu, '.' ile bitir
Merhaba dünya
.
defter> liste          # numaralı satırlar
defter> sil 1          # satır sil
defter> kaydet
defter> çık
tamga@cihaz:/$ göster /belgeler/notlar.txt
```
> MinGW araçları PATH'te değilse: `set PATH=C:\ProgramData\mingw64\mingw64\bin;%PATH%`

`mingw32-make` yoksa doğrudan gcc ile (PowerShell örneği `docs/` içinde):
```
gcc -std=c11 -I src <tüm .c kaynaklar except hal_pico.c> -o build\tamga.exe -lm
```

## Raspberry Pi Pico W'ye yükleme

ARM toolchain + Pico SDK gerekir. Adımlar: **[docs/pico-port.md](docs/pico-port.md)**.
```bat
set PICO_SDK_PATH=C:/pico/pico-sdk
cmake -B build-pico -G "Ninja" .
cmake --build build-pico          :: build-pico\tamga.uf2
```
`.uf2`'yi BOOTSEL modundaki Pico'ya sürükle-bırak.

## ESP32'ye (ESP32-WROOM-32) yükleme

İki yol var:

**A) Arduino IDE (en kolay)** — Adımlar: **[docs/arduino-port.md](docs/arduino-port.md)**.
ESP32 core'u kur, bu klasörü (`tamgaOS.ino`) aç, "ESP32 Dev Module" seç, Yükle.
Arduino `src/` klasörünü özyineli derler; WiFi (`wifi-tara`/`wifi-baglan`) gerçek çalışır.

**B) ESP-IDF** — Adımlar: **[docs/esp32-port.md](docs/esp32-port.md)**.
```bat
cd esp32
idf.py set-target esp32
idf.py build
idf.py -p COM3 flash monitor
```

## Dizin yapısı

```
src/cekirdek/   çekirdek (zamanlayıcı, bellek, olay, syscall)
src/hal/        HAL: hal.h + hal_host.c (PC) + hal_pico.c (Pico)
src/tds/        Tamga Dosya Sistemi
src/tamga/      dil: tarayıcı, ayrıştırıcı, yorumlayıcı, moduller/
src/kabuk/      kabuk REPL + komutlar
uygulamalar/    örnek .tam uygulamalar
testler/        birim testleri
docs/           mimari, dil spesifikasyonu, Pico port planı
```

## Belgeler

- [docs/mimari.md](docs/mimari.md) — katmanlar, veri akışı, bellek bütçesi
- [docs/tamga-dili.md](docs/tamga-dili.md) — dil spesifikasyonu ve gramer
- [docs/pico-port.md](docs/pico-port.md) — Pico W derleme ve flash
- [docs/esp32-port.md](docs/esp32-port.md) — ESP32 (ESP-IDF) derleme ve flash
- [docs/arduino-port.md](docs/arduino-port.md) — ESP32'ye Arduino IDE ile derleme

## Örnek Tamga kodu

```tamga
işlev faktoriyel(n)
    sonuc = 1
    i = 1
    iken i <= n
        sonuc = sonuc * i
        i = i + 1
    bitir
    döndür sonuc
bitir

yaz "5! = " + faktoriyel(5)     # 5! = 120
```

## Durum

Sürüm 0.1 — **çalışan dikey dilim**: çekirdek + TDS + kabuk + tam dil
(yaz/değişken/aritmetik/eğer/iken/işlev/modüller) PC'de çalışır; Pico W yapı
dosyaları hazırdır. Sonraki adımlar `docs/mimari.md` sonunda listelenir.
>>>>>>> 3c79836 (v0.1)
