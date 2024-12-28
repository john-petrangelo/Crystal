set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

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
        src/Network.cpp
        src/Renderer.cpp
        src/Status.cpp
        src/System.cpp

        src/lib/lumos-arduino/Models/Crystal.h
        src/lib/lumos-arduino/Models/Crystal.cpp
        src/lib/lumos-arduino/Models/Flame.h
        src/lib/lumos-arduino/Models/Flame.cpp
        src/lib/lumos-arduino/Models/Gradient.h
        src/lib/lumos-arduino/Models/Gradient.cpp
        src/lib/lumos-arduino/Models/JacobsLadder.h
        src/lib/lumos-arduino/Models/JacobsLadder.cpp
        src/lib/lumos-arduino/Models/Map.h
        src/lib/lumos-arduino/Models/Map.cpp
        src/lib/lumos-arduino/Models/Model.h
        src/lib/lumos-arduino/Models/Model.cpp
        src/lib/lumos-arduino/Models/Pulsate.h
        src/lib/lumos-arduino/Models/Pulsate.cpp
        src/lib/lumos-arduino/Models/Rotate.h
        src/lib/lumos-arduino/Models/Rotate.cpp
        src/lib/lumos-arduino/Models/Shift.h
        src/lib/lumos-arduino/Models/Shift.cpp
        src/lib/lumos-arduino/Models/Sum.h
        src/lib/lumos-arduino/Models/Sum.cpp
        src/lib/lumos-arduino/Models/Triangle.h
        src/lib/lumos-arduino/Models/Triangle.cpp
        src/lib/lumos-arduino/Models/WarpCore.h
        src/lib/lumos-arduino/Models/WarpCore.cpp
        src/lib/lumos-arduino/Models/Window.h
        src/lib/lumos-arduino/Models/Window.cpp

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
        src/common
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

        ${ESP8266_LIB_DIR}/tools/sdk/include
)
