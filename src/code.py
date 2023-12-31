import os
import time

import board

from luminaria.colors import BLUE, GREEN, ORANGE, RED, VIOLET, WHITE, YELLOW
from luminaria.models import MultiGradient, Solid
from luminaria.animations import Rotate
from luminaria.renderer.circuitpython_neopixel_renderer import Renderer

import network

# Setup network
wifi_ssid = os.getenv("WIFI_SSID")
wifi_password = os.getenv("WIFI_PASSWORD")
network.setup_mdns()
ip_address = network.connect_wifi(wifi_ssid, wifi_password)
server = network.start_webserver(ip_address)

# Setup neopixel sequence
print("Initializing pixels")
PIXELS_PIN = board.GP28
PIXELS_COUNT = 50
BRIGHTNESS = 1.0
pixels = Renderer(PIXELS_PIN, PIXELS_COUNT, brightness=BRIGHTNESS)

# Flash the pixels white for a short time
pixels.model = Solid("Solid white", WHITE)
pixels.render()
time.sleep(0.2)

# Set up the model for the pixels
print("Setting up lighting model")
gradient = MultiGradient("Gradient rainbow", [RED, ORANGE, YELLOW, GREEN, BLUE, VIOLET, RED])
rotation = Rotate("Rotation", 2500, gradient)
pixels.model = rotation

print("Starting loop")
running_average = 0.0
while True:
    server.poll()

    before_time_ns = time.monotonic_ns()
    pixels.render()
    after_time_ns = time.monotonic_ns()

    duration = (after_time_ns - before_time_ns) / 1000000000
    network.report_run_time(duration)

    time.sleep(0.010)
