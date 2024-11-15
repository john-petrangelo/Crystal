# crystal
A glowing crystal project using an ESP-01s and Neopixels

Uses the library at https://github.com/john-petrangelo/lumos-arduino.

NOTE:
The build relies on files from the https://github.com/john-petrangelo/lumos-arduino.git repo. You must
clone this repo locally and then make a symbolic link at esp8266/src/lumos-arduino which points to the
local repo.

TODO: I need to fix this so that it's not reaching into the esp8266 directory.

# Build for ESP8266 using Arduino tools

To compile the code, run the build script in the esp8266 directory:
```
> build.sh
```

To upload to device over serial:
```
> arduino-cli upload -p /dev/cu.usbserial-114230 --fqbn esp8266:esp8266:generic Crystal.ino
```

To upload to device over network OTA:
```
> upload-ota.sh
```
# Build for Raspberry Pi Pico W

## Environment variables

You will need to set the following environment variables before calling cmake:
- PICO_BOARD=pico_w
- PICO_SDK_PATH=/Users/john/Documents/Programming/rpi_pico/pico-sdk

Unused so far...
- PICO_EXTRAS_PATH=/Users/john/Documents/Programming/rpi_pico/pico-extras
- PICO_EXAMPLES_PATH=/Users/john/Documents/Programming/rpi_pico/pico-examples

TODO Write up cmake instructions

# License
Some files in this project have been copied from other projects in compliance with the MIT License.
