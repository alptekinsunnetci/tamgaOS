# esp32/CMakeLists.txt — Tamga OS, ESP32 (ESP-IDF) proje tanimi.
# Kurulum ve flash icin: ../docs/esp32-port.md
# Ozet:
#   cd esp32
#   idf.py set-target esp32
#   idf.py build
#   idf.py -p COM3 flash monitor
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(tamga_os)
