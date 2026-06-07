# Tamga OS — Mimari

Tamga OS, gömülü kartlar (RP2040, ESP32) ve geliştirme için PC üzerinde çalışan,
**tamamen Türkçe**, Unix felsefesinden ilham alan minimal bir işletim sistemidir.
Kullanıcı uygulamaları C binary değildir; `.tam` dosyaları olarak **Tamga Dili**
ile yazılır ve çekirdek içindeki ağaç-yürüyen yorumlayıcıda çalıştırılır.

## Katmanlar

```
        +-------------------------------------------------+
   K    |  KABUK (kabuk/)                                  |
   u    |  Türkçe REPL · Unix komutları · defter editörü   |
   l    +-------------------------------------------------+
   l    |  TAMGA DİLİ (tamga/)                             |
   a    |  tarayıcı → ayrıştırıcı → yorumlayıcı            |
   n    |  değerler (sayı/metin/dizi/sözlük) + moduller    |
   ı    +-------------------------------------------------+
   c    |  TDS — Tamga Dosya Sistemi (tds/)                |
   ı    |  oku · yaz · sil · listele · KALICI (serileştirme)|
        +-------------------------------------------------+
        |  ÇEKİRDEK (cekirdek/)                            |
        |  zamanlayıcı · bellek · olay · syscall           |
        +-------------------------------------------------+
        |  HAL (hal/hal.h)                                |
        +-------------+-------------+-----------+-----------+
        | hal_host.c  | hal_pico.c  | hal_esp32 | hal_      |
        | (PC/MinGW)  | (RP2040)    | .c (IDF)  | arduino.cpp|
        +-------------+-------------+-----------+-----------+
        | stdio·dosya | USB·CYW43   | UART·     | Serial·   |
        | flash       | WiFi        | esp_wifi  | WiFi·LFS  |
        +-------------+-------------+-----------+-----------+
```

Donanıma bağlı **her şey** `hal.h` arkasındadır; üst katmanlar yalnızca HAL'i
çağırır. Platform seçimi derleme bayrağıyla yapılır: host (bayrak yok),
`TAMGA_PICO`, `TAMGA_ESP32`, `ARDUINO` (Arduino IDE otomatik tanımlar).

## Giriş noktası (platforma göre)

| Platform   | Giriş            | Dosya |
|------------|------------------|-------|
| Host/Pico  | `main()`         | `src/ana.c` |
| ESP-IDF    | `app_main()`     | `src/ana.c` (`TAMGA_ESP32`) |
| Arduino    | `setup()`/`loop()`| `tamgaOS.ino` (`loop()` → `zamanlayici_tik()`) |

## Veri akışı: bir satırın işlenişi

```
"yaz 2 + 3 * 4"
   │  tarayıcı   → [YAZ][SAYI 2][ARTI][SAYI 3][CARPI][SAYI 4][DOSYASONU]
   │  ayrıştırıcı → D_YAZ( D_IKILI(+, 2, D_IKILI(*, 3, 4)) )
   │  yorumlayıcı → ağacı yürüt → 14 → hal_uart_yaz("14\n")
```

## Çekirdek bileşenleri

| Bileşen     | Dosya                    | Görev |
|-------------|--------------------------|-------|
| Bellek      | `cekirdek/bellek.c`      | Heap'ten ayrılan arena üzerinde serbest-listeli ayırıcı (libc malloc'a bağımlı çalışma-zamanı yok) |
| Zamanlayıcı | `cekirdek/zamanlayici.c` | Kooperatif round-robin görev tablosu |
| Olay        | `cekirdek/olay.c`        | Halka tampon mesaj kuyruğu |
| Syscall     | `cekirdek/syscall.c`     | Modüllerin HAL/çekirdeğe eriştiği ince kapı |
| Çekirdek    | `cekirdek/cekirdek.c`    | Alt sistemleri başlatır, ana döngü, TDS yükle/tohum |

## Bellek yapılandırması

Boyutlar `src/tamga_yapilandirma.h` içinde platforma göre tanımlıdır. Gömülü
hedeflerde (`TAMGA_GOMULU`) statik RAM (.bss) kısıtlıdır — özellikle ESP32'de
WiFi yığını DRAM'in çoğunu alır — bu yüzden değerler küçülür ve arena heap'ten
ayrılır.

| Ayar                | Host  | Gömülü | Açıklama |
|---------------------|-------|--------|----------|
| `TAMGA_ARENA_BOYUT` | 96 KB | 48 KB  | Yorumlayıcı arenası (heap'ten) |
| `DEGER_METIN_AZAMI` | 128   | 64     | Bir metin değerinin azami baytı |
| `ORTAM_AZAMI`       | 32    | 16     | Kapsam başına değişken sayısı |
| `ORTAM_AD_AZAMI`    | 32    | 24     | Değişken adı uzunluğu |
| `TDS_AZAMI_DOSYA`   | 32    | 12     | Azami dosya sayısı |
| `TDS_AZAMI_BOYUT`   | 2048  | 1024   | Dosya başına bayt |
| `TDS_FLASH_BOYUT`   | 96 KB | 16 KB  | Kalıcı imaj aynası |

**Yığın notu (ESP32):** Yorumlayıcı özyinelidir ve işlev çağrısında `Ortam`
heap'ten ayrılır. Arduino'da `loopTask` yığını `SET_LOOP_TASK_STACK_SIZE(24KB)`
ile büyütülür (varsayılan 8 KB yetmez).

## Kalıcılık

TDS, `hal_flash_*` blok aygıtı üzerine basit bir formatla serileştirilir
(`tds_kaydet`/`tds_yukle`) ve her yazma işleminde otomatik kaydedilir.
- **Host:** `tamga_flash.bin` dosyası.
- **Arduino/ESP32:** LittleFS `/tamga.img`.
- **Pico/ESP-IDF:** flush şu an yer tutucu (kalıcılık ileride).

Açılışta geçerli imaj varsa yüklenir; yoksa örnek uygulamalar tohumlanır.

## Tasarım ilkeleri

- **Türkçe-öncelikli:** anahtar kelimeler, komutlar, hata mesajları Türkçe.
- **Gömülü-öncelikli:** öngörülebilir bellek, küçük yığın izi, platforma göre ayar.
- **Unix esinli:** küçük, birleşebilen parçalar; her şey dosya/komut.
- **Taşınabilir çekirdek + ince HAL:** donanım bağımlılığı tek sınırda.

## Yol haritası

1. ✅ **Kalıcılık** — TDS `hal_flash` üzerine serileştirilir (host dosya, Arduino LittleFS).
2. ✅ **Diziler** — `[...]`, indeksleme, `uzunluk/ekle/çıkar/dizi`.
3. ✅ **Sözlükler** — `{...}`, `s["k"]`, `anahtarlar/var_mı/sil_anahtar/sözlük`.
4. ✅ **Metin işlevleri** — `parça/bul/içerir/başlar/biter/kırp/değiştir/parçala/birleştir`.
5. ✅ **Döngü kontrolü** — `her` (for-each), `dur` (break), `devam` (continue).
6. Gerçek WiFi: ESP-IDF tarafı `esp_wifi_scan` (Arduino'da zaten gerçek).
7. Pico/ESP-IDF kalıcı flash (littlefs / NVS).
8. `değilse eğer` zinciri, matematik modülü, REPL çok-satır modu.
9. Çoklu görev: kabuk + arka plan `.tam` uygulamaları aynı zamanlayıcıda.
