#!/bin/bash

# A short script that builds the image for ESP8266 using the arduino-cli tool

# Set up the fully-qualified board name
PLATFORM=esp8266
BOARD=esp8266
VARIANT=generic
FQBN="$PLATFORM:$BOARD:$VARIANT"

# Add lib directory to include path
BUILD_PROPERTY='build.extra_flags="-I../lib"'

# Build the image
arduino-cli compile --fqbn $FQBN --build-property "$BUILD_PROPERTY" -v esp8266.ino
