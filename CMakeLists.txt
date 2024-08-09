cmake_minimum_required(VERSION 3.27)

# Check for the PICO_SDK_PATH environment variable and if found include the Pico SDK
if(DEFINED ENV{PICO_SDK_PATH})
    include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
endif()

# Define the project name and supported languages
project(Crystal C CXX ASM)

# Add the Raspberry Pi Pico subdirectory if the SDK path is defined
if(DEFINED ENV{PICO_SDK_PATH})
    add_subdirectory(raspberry-pi-pico)
endif()

# Add the ESP8266 subdirectory if the ESP8266 variable is defined
if(ESP8266)
    add_subdirectory(esp8266)
endif()
