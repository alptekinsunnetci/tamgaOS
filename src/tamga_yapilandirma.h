/* tamga_yapilandirma.h — derleme zamani boyut ayarlari (bellek ayak izi).
 *
 * Gomulu hedeflerde (ESP32/Arduino/Pico) RAM ve ozellikle statik .bss kisitlidir
 * (ESP32'de WiFi yigini cogu DRAM'i alir). Bu yuzden tamponlar kuculur; arena
 * ayrica heap'ten ayrilir (bkz. bellek.c). Host'ta bol kaynak: buyuk degerler.
 */
#ifndef TAMGA_YAPILANDIRMA_H
#define TAMGA_YAPILANDIRMA_H

#if defined(ARDUINO) || defined(TAMGA_ESP32) || defined(TAMGA_PICO)
#  define TAMGA_GOMULU 1
#endif

#if defined(TAMGA_GOMULU)
   /* --- Gomulu (ESP32 / Pico) --- */
#  define TAMGA_ARENA_BOYUT   (48u * 1024u)  /* yorumlayici arenasi (heap'ten) */
#  define TDS_AZAMI_DOSYA     12
#  define TDS_AZAMI_BOYUT     1024
#  define TDS_FLASH_BOYUT     (16u * 1024u)  /* kalici imaj aynasi */
#  define DEFTER_MAKS_SATIR   24
#  define DEFTER_SATIR_UZ     80
   /* Yigin baskisini azalt: kucuk Deger ve Ortam (ESP32 loopTask yigini ~8-24KB) */
#  define DEGER_METIN_AZAMI   64
#  define ORTAM_AZAMI         16
#  define ORTAM_AD_AZAMI      24
#else
   /* --- Host (PC) --- */
#  define TAMGA_ARENA_BOYUT   (96u * 1024u)
#  define TDS_AZAMI_DOSYA     32
#  define TDS_AZAMI_BOYUT     2048
#  define TDS_FLASH_BOYUT     (96u * 1024u)
#  define DEFTER_MAKS_SATIR   48
#  define DEFTER_SATIR_UZ     96
#  define DEGER_METIN_AZAMI   128
#  define ORTAM_AZAMI         32
#  define ORTAM_AD_AZAMI      32
#endif

#endif /* TAMGA_YAPILANDIRMA_H */
