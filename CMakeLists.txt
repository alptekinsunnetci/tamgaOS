# CMakeLists.txt — Tamga OS, Raspberry Pi Pico W (RP2040) hedefi.
#
# Kurulum ve flash icin: docs/pico-port.md
# Ozet:
#   set PICO_SDK_PATH=C:/pico/pico-sdk        (Pico SDK yolu)
#   cmake -B build-pico -G "Ninja" .
#   cmake --build build-pico                  -> build-pico/tamga.uf2
#
# NOT: Host (PC) yapisi icin CMake DEGIL, Makefile kullanilir (MinGW gcc).

cmake_minimum_required(VERSION 3.13)

# Pico W: kablosuz icin cyw43 surucusu gerekir
set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

project(tamga_os C CXX ASM)

pico_sdk_init()

add_executable(tamga
    src/ana.c
    src/cekirdek/cekirdek.c
    src/cekirdek/bellek.c
    src/cekirdek/olay.c
    src/cekirdek/zamanlayici.c
    src/cekirdek/syscall.c
    src/hal/hal_pico.c
    src/tds/tds_ram.c
    src/tds/tds_tohum.c
    src/tamga/tarayici.c
    src/tamga/agac.c
    src/tamga/deger.c
    src/tamga/dizi.c
    src/tamga/sozluk.c
    src/tamga/metin_islev.c
    src/tamga/ortam.c
    src/tamga/ayristirici.c
    src/tamga/yorumlayici.c
    src/tamga/tamga.c
    src/tamga/moduller/mod_sistem.c
    src/tamga/moduller/mod_dosya.c
    src/tamga/moduller/mod_zaman.c
    src/tamga/moduller/modul_dagitici.c
    src/kabuk/kabuk.c
    src/kabuk/komutlar.c
    src/kabuk/defter.c
)

# Pico HAL'i etkinlestir (hal_pico.c bu tanim olmadan derlenmez)
target_compile_definitions(tamga PRIVATE TAMGA_PICO=1)

target_include_directories(tamga PRIVATE src)

# Seri konsolu USB-CDC uzerinden ver (UART yerine); istenirse degistirilebilir
pico_enable_stdio_usb(tamga 1)
pico_enable_stdio_uart(tamga 0)

target_link_libraries(tamga
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background  # WiFi (CYW43439)
    hardware_gpio
)

# .uf2 / .elf / .map ciktilarini uret (suruklenip Pico'ya birakilir)
pico_add_extra_outputs(tamga)
