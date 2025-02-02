###############################
# Raspberry Pi Pico W
###############################

cmake_minimum_required(VERSION 3.27)

pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Define the executable target
add_executable(crystal-pi-pico
        Crystal.cpp
        Demos.h
        Demos.cpp
        DHCPServer.h
        DHCPServer.cpp
        Handlers.h
        Handlers.cpp
        Network.h
        Network.cpp
        Renderer.h
        Renderer.cpp
        Status.h
        Status.cpp
        System.h
        System.cpp
        Utils.h
        Utils.cpp
        WS2812Strip.h
        WS2812Strip.cpp
        ws2812.pio

        ${CMAKE_SOURCE_DIR}/lib/lumos-arduino/Colors.cpp
#        ${CMAKE_SOURCE_DIR}/lib/lumos-arduino/Patterns.cpp
)

# Generate header file from PIO source
pico_generate_pio_header(crystal-pi-pico ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

# Add network and console logging classes to the target
target_sources(crystal-pi-pico PRIVATE
        Logging/ConsoleLogger.h
        Logging/LogServer.h
        Logging/LogServer.cpp
        Logging/NetworkLogger.h
)

# Add HTTPServer classes to the target
target_sources(crystal-pi-pico PRIVATE
        HTTP/ConnectionContext.h
        HTTP/ConnectionContext.cpp
        HTTP/HTTPRequest.h
        HTTP/HTTPRequest.cpp
        HTTP/HTTPRequestParser.h
        HTTP/HTTPRequestParser.cpp
        HTTP/HTTPResponse.h
        HTTP/HTTPServer.h
        HTTP/HTTPServer.cpp
)

# Add model classes to the target
target_sources(crystal-pi-pico PRIVATE
        ../lib/lumos-arduino/Models/Model.h
        ../lib/lumos-arduino/Models/Model.cpp
        ../lib/lumos-arduino/Models/ModelUtils.h
        ../lib/lumos-arduino/Models/ModelUtils.cpp
        ../lib/lumos-arduino/Models/Crystal.h
        ../lib/lumos-arduino/Models/Crystal.cpp
        ../lib/lumos-arduino/Models/Flame.h
        ../lib/lumos-arduino/Models/Flame.cpp
        ../lib/lumos-arduino/Models/Gradient.h
        ../lib/lumos-arduino/Models/Gradient.cpp
        ../lib/lumos-arduino/Models/JacobsLadder.h
        ../lib/lumos-arduino/Models/JacobsLadder.cpp
        ../lib/lumos-arduino/Models/Map.h
        ../lib/lumos-arduino/Models/Map.cpp
        ../lib/lumos-arduino/Models/Pulsate.h
        ../lib/lumos-arduino/Models/Pulsate.cpp
        ../lib/lumos-arduino/Models/Rotate.h
        ../lib/lumos-arduino/Models/Rotate.cpp
        ../lib/lumos-arduino/Models/Shift.h
        ../lib/lumos-arduino/Models/Shift.cpp
        ../lib/lumos-arduino/Models/Solid.h
        ../lib/lumos-arduino/Models/Sum.h
        ../lib/lumos-arduino/Models/Sum.cpp
        ../lib/lumos-arduino/Models/Triangle.h
        ../lib/lumos-arduino/Models/Triangle.cpp
        ../lib/lumos-arduino/Models/WarpCore.h
        ../lib/lumos-arduino/Models/WarpCore.cpp
        ../lib/lumos-arduino/Models/Window.h
        ../lib/lumos-arduino/Models/Window.cpp
)

# Add logging sources to the target
target_sources(crystal-pi-pico PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/lumos-arduino/Logger.cpp
)

# Add generated web data files to the target
target_sources(crystal-pi-pico PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/web_files.h
        ${CMAKE_CURRENT_BINARY_DIR}/web_files.cpp
)

# Add include directories
target_include_directories(crystal-pi-pico PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lib/ArduinoJson/src
        ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(crystal-pi-pico
        pico_stdlib
        pico_cyw43_arch_lwip_poll
)

# Include the mDNS source files
target_sources(crystal-pi-pico PRIVATE
    ${PICO_SDK_PATH}/lib/lwip/src/apps/mdns/mdns.c
    ${PICO_SDK_PATH}/lib/lwip/src/apps/mdns/mdns_out.c
    ${PICO_SDK_PATH}/lib/lwip/src/apps/mdns/mdns_domain.c
)

# Add extra outputs for the Raspberry Pi Pico
pico_add_extra_outputs(crystal-pi-pico)

# The static web files
set(DATA_FILES
        ${CMAKE_SOURCE_DIR}/data/crystal.css
        ${CMAKE_SOURCE_DIR}/data/crystal.js
        ${CMAKE_SOURCE_DIR}/data/index.html
)

# The static web files after converting to C code
set(DATA_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/web_files.h)
set(DATA_SOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/web_files.cpp)

# Command to convert web files into C source code
add_custom_command(
        OUTPUT ${DATA_HEADER_FILE} ${DATA_SOURCE_FILE}
        COMMAND ${CMAKE_COMMAND} -E env python3 ${CMAKE_CURRENT_SOURCE_DIR}/embed_files.py
        DEPENDS ${DATA_FILES} embed_files.py
        COMMENT "Generating web_files.h and webfiles.cpp"
)

# Custom target to ensure the files are generated before building
add_custom_target(generate_data_files
        DEPENDS ${DATA_HEADER_FILE} ${DATA_SOURCE_FILE}
)

# Make sure the generate_data_files target runs before the main build target
add_dependencies(crystal-pi-pico generate_data_files)
