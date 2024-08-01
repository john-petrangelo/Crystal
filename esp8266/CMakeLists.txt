set(CMAKE_CXX_STANDARD 14)
set(HOME $ENV{HOME})

add_compile_definitions(
        ARDUINO_ARCH_ESP8266
        ARDUINOJSON_ENABLE_ARDUINO_STRING=1
)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/esp8266)

set(SRC_FILES
        ${SRC_DIR}/esp8266.ino
        ${SRC_DIR}/src/Models/Window.cpp
        ${SRC_DIR}/src/Demos.cpp
        ${SRC_DIR}/src/Filesystem.cpp
        ${SRC_DIR}/src/Handlers.cpp
        ${SRC_DIR}/src/Models/Gauge.cpp
        ${SRC_DIR}/src/Network.cpp
        ${SRC_DIR}/src/Renderer.cpp
        ${SRC_DIR}/src/Status.cpp
        ${SRC_DIR}/src/System.cpp
        ${SRC_DIR}/src/lumos-arduino/Colors.cpp
        ${SRC_DIR}/src/lumos-arduino/Logger.cpp
        ${SRC_DIR}/src/lumos-arduino/Patterns.cpp
        ${SRC_DIR}/src/Models/Crystal.cpp
        ${SRC_DIR}/src/Models/Flame.cpp
        ${SRC_DIR}/src/Models/Gradient.cpp
        ${SRC_DIR}/src/Models/JacobsLadder.cpp
        ${SRC_DIR}/src/Models/Map.cpp
        ${SRC_DIR}/src/Models/Model.cpp
        ${SRC_DIR}/src/Models/Pulsate.cpp
        ${SRC_DIR}/src/Models/Rotate.cpp
        ${SRC_DIR}/src/Models/Shift.cpp
        ${SRC_DIR}/src/Models/Sum.cpp
        ${SRC_DIR}/src/Models/Triangle.cpp
        ${SRC_DIR}/src/Models/WarpCore.cpp
)

add_executable(crystal-esp8266x ${SRC_FILES})

target_include_directories(crystal-esp8266x PRIVATE
        src
        ${HOME}/Documents/Arduino/libraries/NeoPixelBus_by_Makuna/src
        ${HOME}/Documents/Arduino/Libraries
        ${HOME}/Documents/Arduino/libraries/ArduinoJson/src
        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/cores/esp8266
        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ArduinoOTA
        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266mDNS/src
        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WebServer/src
        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266Wifi/src
        ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/LittleFS/src
)

# Add a custom command that runs the shell script
add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/arduino-cli-output"
        COMMAND arduino-cli compile --fqbn esp8266:esp8266:generic -v ${SRC_DIR}/esp8266.ino
        DEPENDS ${SRC_FILES}
        COMMENT "Running arduino-cli command"
)

# Add a custom target that depends on the output of the custom command
add_custom_target(crystal-arduino-esp8266 ALL DEPENDS "${CMAKE_BINARY_DIR}/arduino-cli-output")
