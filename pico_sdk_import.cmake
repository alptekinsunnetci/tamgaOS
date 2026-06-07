# pico_sdk_import.cmake — standart Pico SDK import shim.
# Pico SDK'yi PICO_SDK_PATH ortam degiskeni veya -DPICO_SDK_PATH ile bulur.
# (Bu, pico-sdk/external/pico_sdk_import.cmake dosyasinin kopyasidir.)

if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    message("PICO_SDK_PATH ortam degiskeninden alindi: ${PICO_SDK_PATH}")
endif ()

if (NOT PICO_SDK_PATH)
    message(FATAL_ERROR
        "PICO_SDK_PATH tanimli degil. Pico SDK'yi kurun ve PICO_SDK_PATH'i ayarlayin.\n"
        "Bkz. docs/pico-port.md")
endif ()

get_filename_component(PICO_SDK_PATH "${PICO_SDK_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH bulunamadi: '${PICO_SDK_PATH}'")
endif ()

set(PICO_SDK_INIT_CMAKE_FILE ${PICO_SDK_PATH}/pico_sdk_init.cmake)
if (NOT EXISTS ${PICO_SDK_INIT_CMAKE_FILE})
    message(FATAL_ERROR "Gecersiz PICO_SDK_PATH '${PICO_SDK_PATH}' — pico_sdk_init.cmake yok")
endif ()

include(${PICO_SDK_INIT_CMAKE_FILE})
