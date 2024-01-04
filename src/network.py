import time

from ipaddress import IPv4Address

from adafruit_httpserver import Server, Request, JSONResponse

import mdns
import microcontroller
import wifi

import socketpool

running_average = 0.0


def report_run_time(run_time):
    global running_average
    running_average = running_average * 0.9 + run_time * 0.1


def disconnect_networks():
    print("Stopping previous wifi networks")
    wifi.radio.stop_ap()
    wifi.radio.stop_station()


def setup_soft_ap():
    print("Starting access point")
    ap_ssid = "Raspberry Pi Pico"
    ap_password = "password"
    wifi.radio.start_ap(ap_ssid, password=ap_password)

    ip_address = wifi.radio.ipv4_address_ap
    hostname = wifi.radio.hostname
    print(f"AP started, ip={ip_address} hostname={hostname}")


def connect_wifi(ssid: str, password: str) -> IPv4Address:
    print(f"Connecting to {ssid}")
    wifi.radio.connect(ssid, password)
    ip_address = wifi.radio.ipv4_address
    hostname = wifi.radio.hostname
    print(f"Connected to {ssid}, ip={ip_address} hostname={hostname}")

    return ip_address


def setup_mdns():
    mdns_server = mdns.Server(wifi.radio)
    mdns_server.hostname = "leds"
    mdns_server.advertise_service(service_type="_http", protocol="_tcp", port=80)
    print(f"Started MDNS server, connect to {mdns_server.hostname}.local")


def start_webserver(ip_address: IPv4Address) -> Server:
    print("Creating web server")
    pool = socketpool.SocketPool(wifi.radio)
    server = Server(pool, "/static", debug=True)

    @server.route("/info", methods=["GET"])
    def handle_request(request: Request):
        data = {
            "temperature": microcontroller.cpu.temperature,
            "frequency": microcontroller.cpu.frequency,
            "voltage": microcontroller.cpu.voltage,
            "time": time.monotonic(),
            "avgDuration": running_average,
        }
        return JSONResponse(request, data)

    # Start the web server
    print("Starting web server")
    server.start(str(ip_address))

    return server
