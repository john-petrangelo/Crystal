###############################
# Raspberry Pi Pico W
###############################

cmake_minimum_required(VERSION 3.27)

pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Define the executable target
add_executable(crystal-pi-pico
        ConsoleLogger.h
        Crystal.cpp
        Handlers.h
        Handlers.cpp
        LogServer.h
        LogServer.cpp
        Network.h
        Network.cpp
        System.h
        System.cpp
        Utils.h
        Utils.cpp
)

# Add HTTPServer classes to the target
target_sources(crystal-pi-pico PRIVATE
        HTTP/HTTPRequest.h
        HTTP/HTTPRequestParser.cpp
        HTTP/HTTPRequestParser.h
        HTTP/HTTPServer.h
        HTTP/HTTPServer.cpp
)

# Add arduino-json sources to the target
target_sources(crystal-pi-pico PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/lumos-arduino/Logger.cpp
)

# Add include directories
target_include_directories(crystal-pi-pico PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/lib/ArduinoJson/src
        ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(crystal-pi-pico
        pico_stdlib
        pico_cyw43_arch_lwip_poll
)

# Add extra outputs for the Raspberry Pi Pico
pico_add_extra_outputs(crystal-pi-pico)
