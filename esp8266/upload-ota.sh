#!/bin/bash

# A short script that uploads the ESP8266 image via OTA to target using the arduino-cli tool

# Set up the fully-qualified board name
PLATFORM=esp8266
BOARD=esp8266
VARIANT=generic
FQBN="$PLATFORM:$BOARD:$VARIANT"

# The IP address of the target to be updated
IP_ADDR=192.168.4.169

# Upload the image using OTA
arduino-cli upload --fqbn $FQBN --protocol=network --port "$IP_ADDR" esp8266.ino
