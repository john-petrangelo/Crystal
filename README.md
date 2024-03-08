# crystal
A glowing crystal project using an ESP-01s and Neopixels

Uses the library at https://github.com/john-petrangelo/lumos-arduino.


# Dev tips

To compile the code:
```
arduino-cli compile --fqbn esp8266:esp8266:generic -v Crystal.ino
```

To upload to device over serial:
```
arduino-cli upload -p /dev/cu.usbserial-114230 --fqbn esp8266:esp8266:generic Crystal.ino
```

To upload to device over network OTA:
```
arduino-cli upload --fqbn esp8266:esp8266:generic  --protocol=network --port 192.168.4.169 Crystal.ino
```
