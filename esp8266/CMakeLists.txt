# Set home and library directories
set(HOME $ENV{HOME})
set(ARDUINO_LIB_DIR ${HOME}/Documents/Arduino/libraries)
set(ESP8266_LIB_DIR ${HOME}/Library/Arduino15/packages/esp8266/hardware/esp8266/3.1.2)

# Define source files for the project
set(SRC_FILES
        esp8266.ino

        src/ArduinoStreamLogger.cpp
        src/Demos.cpp
        src/Filesystem.cpp
        src/Handlers.cpp
        src/Models/Gauge.cpp
        src/Network.cpp
        src/Renderer.cpp
        src/Status.cpp
        src/System.cpp

        src/Models/Crystal.cpp
        src/Models/Flame.cpp
        src/Models/Gradient.cpp
        src/Models/JacobsLadder.cpp
        src/Models/Map.cpp
        src/Models/Model.cpp
        src/Models/Pulsate.cpp
        src/Models/Rotate.cpp
        src/Models/Shift.cpp
        src/Models/Sum.cpp
        src/Models/Triangle.cpp
        src/Models/WarpCore.cpp
        src/Models/Window.cpp

        src/lib/lumos-arduino/Colors.cpp
        src/lib/lumos-arduino/Logger.cpp
        src/lib/lumos-arduino/Patterns.cpp
)

# Define the executable target
add_executable(crystal-esp8266 ${SRC_FILES})

# Set the C++ standard version
set_target_properties(crystal-esp8266 PROPERTIES CXX_STANDARD 14)

# Add compile definitions
target_compile_definitions(crystal-esp8266 PRIVATE
        ARDUINO_ARCH_ESP8266
        ARDUINOJSON_ENABLE_ARDUINO_STRING=1
)

# Project-specific include directories
target_include_directories(crystal-esp8266 PRIVATE
        src
        src/lib
)
 # External library include directories
target_include_directories(crystal-esp8266 PRIVATE
        ${ARDUINO_LIB_DIR}/libraries/NeoPixelBus_by_Makuna/src
        ${ARDUINO_LIB_DIR}/Libraries
        ${ARDUINO_LIB_DIR}/ArduinoJson/src

        ${ESP8266_LIB_DIR}/cores/esp8266
        ${ESP8266_LIB_DIR}/libraries/ArduinoOTA
        ${ESP8266_LIB_DIR}/libraries/ESP8266mDNS/src
        ${ESP8266_LIB_DIR}/libraries/ESP8266WebServer/src
        ${ESP8266_LIB_DIR}/libraries/ESP8266Wifi/src
        ${ESP8266_LIB_DIR}/libraries/LittleFS/src
)
