# esp32/main/CMakeLists.txt — Tamga OS ana bileseni (ESP-IDF component).
# Kaynaklar repo kokundeki ../../src altindan alinir. hal_esp32.c kullanilir
# (hal_host.c / hal_pico.c HARIC). TAMGA_ESP32 tanimi HAL secimini yapar.

idf_component_register(
    SRCS
        "../../src/ana.c"
        "../../src/cekirdek/cekirdek.c"
        "../../src/cekirdek/bellek.c"
        "../../src/cekirdek/olay.c"
        "../../src/cekirdek/zamanlayici.c"
        "../../src/cekirdek/syscall.c"
        "../../src/hal/hal_esp32.c"
        "../../src/tds/tds_ram.c"
        "../../src/tds/tds_tohum.c"
        "../../src/tamga/tarayici.c"
        "../../src/tamga/agac.c"
        "../../src/tamga/deger.c"
        "../../src/tamga/dizi.c"
        "../../src/tamga/sozluk.c"
        "../../src/tamga/metin_islev.c"
        "../../src/tamga/ortam.c"
        "../../src/tamga/ayristirici.c"
        "../../src/tamga/yorumlayici.c"
        "../../src/tamga/tamga.c"
        "../../src/tamga/moduller/mod_sistem.c"
        "../../src/tamga/moduller/mod_dosya.c"
        "../../src/tamga/moduller/mod_zaman.c"
        "../../src/tamga/moduller/modul_dagitici.c"
        "../../src/kabuk/kabuk.c"
        "../../src/kabuk/komutlar.c"
        "../../src/kabuk/defter.c"
    INCLUDE_DIRS
        "../../src"
    REQUIRES
        driver esp_timer
)

# Pico ve host yerine ESP32 HAL'ini sec
target_compile_definitions(${COMPONENT_LIB} PRIVATE TAMGA_ESP32=1)
