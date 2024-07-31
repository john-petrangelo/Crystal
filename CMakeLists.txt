cmake_minimum_required(VERSION 3.27)

###############################
# Raspberry Pi Pico W
###############################
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(SRC raspberry-pi-pico)

project(Crystal C CXX ASM)

pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(
        crystal-pi-pico
        ${SRC}/Crystal.cpp
        ${SRC}/HttpServer.h
        ${SRC}/HttpServer.cpp
)

target_include_directories(crystal-pi-pico PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/${SRC}
)

target_link_libraries(
        crystal-pi-pico
        pico_stdlib
        pico_cyw43_arch_lwip_poll)

pico_add_extra_outputs(crystal-pi-pico)

###############################
# ESP 8266
###############################

#project(Crystal)
#
#set(CMAKE_CXX_STANDARD 14)
#set(HOME $ENV{HOME})
#set(CMAKE_CXX_COMPILER ${HOME}/Library/Arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/bin/xtensa-lx106-elf-g++)
#
#add_compile_definitions(
#        ARDUINO_ARCH_ESP8266
#        ARDUINOJSON_ENABLE_ARDUINO_STRING=1
#)
#
#include_directories(
#        src
#        ${HOME}/Documents/Arduino/libraries/NeoPixelBus_by_Makuna/src
#        ${HOME}/Documents/Arduino/Libraries
#        ${HOME}/Documents/Arduino/libraries/ArduinoJson/src
#        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/cores/esp8266
#        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ArduinoOTA
#        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266mDNS/src
#        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WebServer/src
#        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266Wifi/src
#        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/LittleFS/src
#)
#
#add_executable(Crystal
#        Crystal.ino
#        src/Models/Window.cpp
#        src/Demos.cpp
#        src/Filesystem.cpp
#        src/Handlers.cpp
#        src/Models/Gauge.cpp
#        src/Network.cpp
#        src/Renderer.cpp
#        src/Status.cpp
#        src/System.cpp
#        src/lumos-arduino/Colors.cpp
#        src/lumos-arduino/Logger.cpp
#        src/lumos-arduino/Patterns.cpp
#        src/Models/Crystal.cpp
#        src/Models/Flame.cpp
#        src/Models/Gradient.cpp
#        src/Models/JacobsLadder.cpp
#        src/Models/Map.cpp
#        src/Models/Model.cpp
#        src/Models/Pulsate.cpp
#        src/Models/Rotate.cpp
#        src/Models/Shift.cpp
#        src/Models/Sum.cpp
#        src/Models/Triangle.cpp
#        src/Models/WarpCore.cpp
#)
