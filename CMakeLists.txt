cmake_minimum_required(VERSION 3.27)

if(DEFINED ENV{PICO_SDK_PATH})
    include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
endif()

project(Crystal C CXX ASM)

if(DEFINED ENV{PICO_SDK_PATH})
    add_subdirectory(raspberry-pi-pico)
endif()

if(ESP8266)
    add_subdirectory(esp8266)
endif()
