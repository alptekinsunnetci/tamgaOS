# Makefile — Tamga OS host (PC / MinGW gcc) yapisi.
# Kullanim:
#   mingw32-make            # build/tamga.exe uretir
#   mingw32-make test       # lexer birim testlerini calistir
#   mingw32-make calistir   # OS'u baslat (REPL)
#   mingw32-make temizle
#
# NOT: Pico W yapisi icin CMakeLists.txt kullanilir (bkz. docs/pico-port.md).

CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -I src -O2
BUILD   := build

# Tum tasinabilir cekirdek + host HAL kaynaklari (hal_pico.c HARIC)
KAYNAK := \
  src/ana.c \
  src/cekirdek/cekirdek.c \
  src/cekirdek/bellek.c \
  src/cekirdek/olay.c \
  src/cekirdek/zamanlayici.c \
  src/cekirdek/syscall.c \
  src/hal/hal_host.c \
  src/tds/tds_ram.c \
  src/tds/tds_tohum.c \
  src/tamga/tarayici.c \
  src/tamga/agac.c \
  src/tamga/deger.c \
  src/tamga/dizi.c \
  src/tamga/sozluk.c \
  src/tamga/metin_islev.c \
  src/tamga/ortam.c \
  src/tamga/ayristirici.c \
  src/tamga/yorumlayici.c \
  src/tamga/tamga.c \
  src/tamga/moduller/mod_sistem.c \
  src/tamga/moduller/mod_dosya.c \
  src/tamga/moduller/mod_zaman.c \
  src/tamga/moduller/modul_dagitici.c \
  src/kabuk/kabuk.c \
  src/kabuk/komutlar.c \
  src/kabuk/defter.c

HEDEF := $(BUILD)/tamga.exe

# Dil testi: tum dil + cekirdek/HAL/TDS kaynaklari (ana/cekirdek/kabuk HARIC)
DIL_KAYNAK := \
  src/cekirdek/bellek.c src/cekirdek/syscall.c src/hal/hal_host.c \
  src/tds/tds_ram.c \
  src/tamga/tarayici.c src/tamga/agac.c src/tamga/deger.c src/tamga/dizi.c src/tamga/sozluk.c src/tamga/metin_islev.c src/tamga/ortam.c \
  src/tamga/ayristirici.c src/tamga/yorumlayici.c src/tamga/tamga.c \
  src/tamga/moduller/mod_sistem.c src/tamga/moduller/mod_dosya.c \
  src/tamga/moduller/mod_zaman.c src/tamga/moduller/modul_dagitici.c

.PHONY: all test lextest diltest calistir temizle

all: $(HEDEF)

$(HEDEF): $(KAYNAK)
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) $(KAYNAK) -o $(HEDEF) -lm

test: lextest diltest

lextest:
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) testler/tarayici_test.c src/tamga/tarayici.c -o $(BUILD)/testler_tarayici.exe -lm
	$(BUILD)/testler_tarayici.exe

diltest:
	@if not exist $(BUILD) mkdir $(BUILD)
	$(CC) $(CFLAGS) testler/dil_test.c $(DIL_KAYNAK) -o $(BUILD)/testler_dil.exe -lm
	$(BUILD)/testler_dil.exe

calistir: $(HEDEF)
	$(HEDEF)

temizle:
	@if exist $(BUILD) rmdir /s /q $(BUILD)
