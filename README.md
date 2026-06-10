# Tamga OS

**Tamga OS**, gömülü kartlar için tasarlanmış, **tamamen Türkçe**, Unix
felsefesinden ilham alan minimal bir işletim sistemidir. Hiçbir kullanıcı
uygulaması C binary olarak çalışmaz; tüm uygulamalar `.tam` dosyaları olarak
**Tamga Dili** ile yazılır ve çekirdek içindeki yorumlayıcıda çalıştırılır.

Aynı taşınabilir çekirdek üç platformda **değişmeden** derlenir:
**PC** (geliştirme/test), **Raspberry Pi Pico W** (RP2040) ve **ESP32** (WROOM-32).

```
==========================================
   TAMGA OS — Türkçe İşletim Sistemi
==========================================
tamga@cihaz:/$ çalıştır merhaba
Merhaba Dünya
Büyük
toplam = 30
tamga@cihaz:/$ yaz 2 + 3 * 4
14
tamga@cihaz:/$ ad = "Tamga"
tamga@cihaz:/$ yaz "Merhaba " + ad
Merhaba Tamga
```

---

## Mimari (katmanlar)

```
   Kabuk (Türkçe REPL + komutlar + defter editörü)
      ▼
   Tamga Dili (tarayıcı → ayrıştırıcı → ağaç-yürüyen yorumlayıcı + modüller)
      ▼
   TDS — Tamga Dosya Sistemi (kalıcı; oku/yaz/sil/listele)
      ▼
   Çekirdek (kooperatif zamanlayıcı · arena bellek · olay kuyruğu · syscall)
      ▼
   HAL (hal.h)  →  hal_host.c | hal_pico.c | hal_esp32.c | hal_arduino.cpp
```

Donanıma bağlı her şey tek bir sınırda (`src/hal/hal.h`) toplanır; her platform
için yalnızca bir HAL dosyası gerekir. Ayrıntı: [docs/mimari.md](docs/mimari.md).

---

## Hızlı başlangıç (PC / Windows + MinGW)

```bat
mingw32-make            :: build\tamga.exe üretir
mingw32-make test       :: tüm testler (lexer 11/11 + dil 72/72)
mingw32-make calistir   :: OS'u başlat (REPL)
mingw32-make temizle
```
> MinGW PATH'te değilse: `set PATH=C:\ProgramData\mingw64\mingw64\bin;%PATH%`

`mingw32-make` yoksa doğrudan `gcc -std=c11 -I src <kaynaklar> -o build\tamga.exe -lm`
(Pico/ESP32 HAL dosyaları hariç — bkz. Makefile).

---

## Tamga Dili — özet

**Değer türleri:** sayı, metin, mantık (`doğru`/`yanlış`), `boş`, dizi, sözlük, işlev

```tamga
# Değişken, aritmetik, koşul
sayi = 10
eğer sayi > 5 ise
    yaz "Büyük"
değilse
    yaz "Küçük"
bitir

# Döngüler: iken + her (for-each) + dur/devam
toplam = 0
her s içinde [10, 20, 30]
    eğer s == 20 ise
        devam
    bitir
    toplam = toplam + s
bitir

# İşlev (ileri bildirimli, özyineli olabilir)
işlev faktoriyel(n)
    eğer n <= 1 ise
        döndür 1
    bitir
    döndür n * faktoriyel(n - 1)
bitir
yaz "5! = " + faktoriyel(5)        # 5! = 120

# Diziler ve sözlükler (başvuru türü)
liste = [1, 2, 3]
ekle(liste, 4)
kisi = {"ad": "Ayşe", "yas": 28}
yaz kisi["ad"] + " " + kisi["yas"]
```

**Yerleşik fonksiyonlar**
- Giriş/dönüşüm: `oku`, `sayı`, `metin`
- Dizi: `dizi`, `uzunluk`, `ekle`, `çıkar`
- Sözlük: `sözlük`, `anahtarlar`, `var_mı`, `sil_anahtar`
- Metin: `parça`, `bul`, `içerir`, `başlar`, `biter`, `kırp`, `değiştir`, `parçala`, `birleştir`

**Sistem modülleri:** `sistem` (bellek/süre), `dosya` (oku/yaz/sil/listele),
`zaman` (simdi/bekle), `gpio` (yön/yaz/oku), `wifi` (tara/bağlan)

Tam spesifikasyon ve gramer: [docs/tamga-dili.md](docs/tamga-dili.md).

---

## Unix-benzeri kabuk

Dosya: `listele` (ls), `gir`/`geri` (cd), `oluştur` (touch), `göster` (cat),
`defter` (editör), `sil` (rm), `kopyala` (cp), `taşı` (mv)
Sistem: `durum`, `bellek`, `süre`, `süreçler`, `wifi-tara`, `wifi-baglan`,
`çalıştır`, `yeniden-baslat`, `yardım`

**Defter** — satır tabanlı metin editörü (`ed`/`nano` ruhunda, seri konsolda çalışır):
```
tamga@cihaz:/$ defter /belgeler/notlar.txt
defter> ekle           # ekleme modu, tek başına '.' ile bitir
Merhaba dünya
.
defter> liste          # numaralı satırlar
defter> sil 1
defter> kaydet
defter> çık
tamga@cihaz:/$ göster /belgeler/notlar.txt
```

**Kalıcılık:** TDS her yazmada kalıcı depoya kaydedilir (PC'de `tamga_flash.bin`,
ESP32/Arduino'da LittleFS) — yazdığın dosya ve uygulamalar **reset sonrası kalır**.

---

## Raspberry Pi Pico W'ye yükleme

ARM toolchain + Pico SDK gerekir. Adımlar: **[docs/pico-port.md](docs/pico-port.md)**.
```bat
set PICO_SDK_PATH=C:/pico/pico-sdk
cmake -B build-pico -G "Ninja" .
cmake --build build-pico          :: build-pico\tamga.uf2
```
`.uf2`'yi BOOTSEL modundaki Pico'ya sürükle-bırak.

## ESP32'ye (ESP32-WROOM-32) yükleme

**A) Arduino IDE (en kolay)** — [docs/arduino-port.md](docs/arduino-port.md):
ESP32 core'u kur, bu klasörü (`tamgaOS.ino`) aç, "ESP32 Dev Module" seç, **Yükle**.
Arduino `src/` klasörünü özyineli derler; WiFi gerçek çalışır.

**B) ESP-IDF** — [docs/esp32-port.md](docs/esp32-port.md):
```bat
cd esp32
idf.py set-target esp32
idf.py build
idf.py -p COM3 flash monitor
```

---

## Dizin yapısı

```
src/cekirdek/   çekirdek (zamanlayıcı, bellek, olay, syscall)
src/hal/        HAL: hal.h + hal_host/hal_pico/hal_esp32/hal_arduino
src/tds/        Tamga Dosya Sistemi (RAM + kalıcı serileştirme)
src/tamga/      dil: tarayıcı, ayrıştırıcı, yorumlayıcı, deger, dizi,
                sozluk, metin_islev, moduller/
src/kabuk/      kabuk REPL + komutlar + defter editörü
src/tamga_yapilandirma.h   platforma göre boyut ayarları
uygulamalar/    örnek .tam uygulamalar
testler/        birim testleri (tarayici_test, dil_test)
docs/           mimari, dil spec, port planları
tamgaOS.ino     Arduino giriş noktası
CMakeLists.txt  Pico SDK yapısı   ·   esp32/  ESP-IDF yapısı   ·   Makefile  host
```

### Örnek uygulamalar (`uygulamalar/`)
`merhaba`, `hesapmakinesi`, `bilgi`, `led`, `diziler`, `sozlukler`, `dongu`,
`metinler`, `selamla`. Açılışta bazıları TDS'e tohumlanır; `çalıştır <ad>` ile çalıştırılır.

---

## Belgeler

- [docs/mimari.md](docs/mimari.md) — katmanlar, veri akışı, bellek bütçesi, yol haritası
- [docs/tamga-dili.md](docs/tamga-dili.md) — dil spesifikasyonu ve gramer
- [docs/pico-port.md](docs/pico-port.md) — Pico W derleme ve flash
- [docs/esp32-port.md](docs/esp32-port.md) — ESP32 (ESP-IDF) derleme ve flash
- [docs/arduino-port.md](docs/arduino-port.md) — ESP32'ye Arduino IDE ile derleme + sorun giderme

---

## Durum

Çalışan, çok platformlu sürüm:
- **Dil:** sayı/metin/mantık/dizi/sözlük/işlev; `eğer/değilse`, `iken`, `her`,
  `dur`/`devam`; zengin yerleşik + modül kümesi.
- **OS:** kalıcı dosya sistemi, `defter` editörü, Unix komutları, kooperatif çekirdek.
- **Platformlar:** PC (MinGW, test edilir), Pico W (Pico SDK), ESP32 (Arduino + ESP-IDF).
- **Testler:** lexer 11/11, dil 72/72.

Sonraki adımlar [docs/mimari.md](docs/mimari.md) sonunda listelenir.
