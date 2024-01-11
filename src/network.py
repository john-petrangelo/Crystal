from ipaddress import IPv4Address

from adafruit_httpserver import Server

import mdns
import wifi

import socketpool

import handlers


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


def start_mdns():
    mdns_server = mdns.Server(wifi.radio)
    mdns_server.hostname = "leds"
    mdns_server.advertise_service(service_type="_http", protocol="_tcp", port=80)
    print(f"Started MDNS server, connect to {mdns_server.hostname}.local")


def start_webserver(ip_address: IPv4Address) -> Server:
    print("Creating web server")
    pool = socketpool.SocketPool(wifi.radio)
    # noinspection PyTypeChecker
    server = Server(pool, "/assets", debug=True)
    handlers.setup_handlers(server)

    # Start the web server
    print("Starting web server")
    server.start(str(ip_address))

    return server
