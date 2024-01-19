import time

import microcontroller
from adafruit_httpserver import Request, JSONResponse, FileResponse, Server, Route, GET, PUT, Response
from luminaria.animations import Rotate, Flame
from luminaria.colors import RED, VIOLET, BLUE, GREEN, YELLOW, ORANGE
from luminaria.models import MultiGradient
from luminaria.renderer.circuitpython_neopixel_renderer import Renderer

from utils import hex_to_color, map_value, make_crystal_model, int24_to_color

renderer: Renderer

running_average = 0.0


def report_render_time(render_time):
    global running_average
    running_average = running_average * 0.9 + render_time * 0.1


def set_renderer(pixels_in):
    global renderer
    renderer = pixels_in


def setup_handlers(server: Server):
    """
    Set up request handlers for the server.

    :param server: The server instance.

    :return: None
    """
    server.add_routes([
        Route("/", GET, handle_root),
        Route("/crystal.css", GET, handle_css),
        Route("/crystal.js", GET, handle_js),
        Route("/brightness", GET, handle_get_brightness),
        Route("/brightness", PUT, handle_set_brightness),
        Route("/crystal", PUT, handle_crystal),
        Route("/rainbow", PUT, handle_rainbow),
        Route("/flame", GET, handle_flame),
        Route("/info", GET, handle_info),
    ])


def handle_root(request: Request):
    return FileResponse(request, "index.html")


def handle_css(request: Request):
    return FileResponse(request, "crystal.css")


def handle_js(request: Request):
    return FileResponse(request, "crystal.js")


def handle_get_brightness(request: Request):
    return JSONResponse(request, {
        "value": renderer.brightness,
    })


def handle_set_brightness(request: Request):
    value = request.query_params["value"]
    if not value:
        return

    # Convert from string to float
    float_value = float(value)

    # Apply gamma correction
    float_value = float_value * float_value

    renderer.brightness = float_value

    return Response(request)


def handle_crystal(request: Request):
    if not request.json():
        return

    upper_color = hex_to_color(request.json()["upper"]["color"])
    upper_speed = float(request.json()["upper"]["speed"])

    middle_color = hex_to_color(request.json()["middle"]["color"])
    middle_speed = float(request.json()["middle"]["speed"])

    lower_color = hex_to_color(request.json()["lower"]["color"])
    lower_speed = float(request.json()["lower"]["speed"])

    upper_period_sec = map_value(upper_speed, 0.0, 1.0, 11.0, 1.0)
    middle_period_sec = map_value(middle_speed, 0.0, 1.0, 11.0, 1.0)
    lower_period_sec = map_value(lower_speed, 0.0, 1.0, 11.0, 1.0)

    print(f"crystal upper_sec={upper_period_sec}")

    renderer.model = make_crystal_model(upper_color, upper_period_sec,
                                        middle_color, middle_period_sec,
                                        lower_color, lower_period_sec)

    return Response(request)


def handle_flame(request: Request):
    renderer.model = Flame("flame")

    return Response(request)


def handle_rainbow(request: Request):
    if not request.json():
        return

    mode = request.json()["mode"]
    speed = float(request.json()["speed"])
    name = None
    colors = []
    if mode == "tv":
        name = "tv-gradient"
        colors = [int24_to_color(i24) for i24 in
                  [0x0000BF, 0xBF0000, 0xBF00C0, 0x00BF00, 0x00BFBE, 0xBFBF00, 0xBFBFBF, 0x666666]]
    elif mode == "50s":
        name = "50s-gradient"
        colors = [int24_to_color(i24) for i24 in [0xB26463, 0x79C874, 0x8AC5BD, 0x9A942E, 0x937535, 0xB26463]]
    elif mode == "60s":
        name = "60s-gradient"
        colors = [int24_to_color(i24) for i24 in [0xFCD53A, 0xBFC847, 0xF7790B, 0xDC4E80, 0x7BD0EC, 0xFCD53A]]
    elif mode == "70s":
        name = "70s-gradient"
        colors = [int24_to_color(i24) for i24 in [0xE59F23, 0x925E18, 0x758C32, 0xCE985A, 0xE15623, 0xE59F23]]
    elif mode == "80s":
        name = "80s-gradient"
        colors = [int24_to_color(i24) for i24 in [0xFE167A, 0x003BB1, 0x00C8AB, 0xFFC800, 0x8000E2, 0xFE167A]]
    elif mode == "90s":
        name = "90s-gradient"
        colors = [int24_to_color(i24) for i24 in [0xB03A50, 0x7F5F14, 0x065B44, 0x4B183C, 0x6B0A29, 0xB03A50]]
    elif mode == "00s":
        name = "00s-gradient"
        colors = [int24_to_color(i24) for i24 in [0x79AED6, 0x8F8C84, 0xC4A051, 0x69322C, 0x3D4D4C, 0x79AED6]]
    elif mode == "10s":
        name = "10s-gradient"
        colors = [int24_to_color(i24) for i24 in [0xF0A2A3, 0x62656D, 0x8E6E29, 0x725A47, 0x4F7018, 0xF0A2A3]]
    else:
        # The "classic" mode. Also use this as a default if we don't recognize the mode.
        _INDIGO = (75, 0, 130)
        name = "vivid-gradient"
        colors = [RED, VIOLET, _INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED]
    print(f"Creating multigradient1, mode={mode} name={name} colors={colors}")

    # colors = [int24_to_color(i24) for i24 in colors]

    print(f"Creating multigradient2, mode={mode} name={name} colors={colors}")
    gm = MultiGradient(name, colors)

    freq = map_value(speed, -1.0, 1.0, -2.0, 2.0)
    model = renderer.model
    if model.name == "rainbow-rotate":
        # Modify the currently running Rotate model
        print(f"Modifying existing rainbow-rotate, speed={speed} freq={freq} gm={gm}")
        model.freq = freq
        model.model = gm
    else:
        # Create a new Rotate model
        print(f"Creating new rainbow-rotate, speed={speed} freq={freq} gm={gm}")
        rm = Rotate("rainbow-rotate", freq, gm)
        renderer.model = rm

    return Response(request)


# void handleSolid() {
#   if(!server.hasArg("color")) {
#     server.send(400, "text/plain", "Color parameter missing\n");
#     return;
#   }
#
#   String colorStr = server.arg("color");
#   Color color = strtol(colorStr.c_str(), 0, 16);
#   renderer.setModel(std::make_shared<SolidModel>("net solid model", color));
#
#   server.send(200, "text/plain", "");
# }
#
#
# void handleDemo1() {
#   renderer.setModel(makeDemo1());
#   server.send(200, "text/plain", "");
# }
#
# void handleDemo2() {
#   renderer.setModel(makeDemo2());
#   server.send(200, "text/plain", "");
# }
#
# void handleDemo3() {
#   renderer.setModel(makeDemo3());
#   server.send(200, "text/plain", "");
# }
#
# void handleNotFound() {
#   String message = "File Not Found\n\n";
#   message += "URI: " + server.uri();
#   message += "\nMethod: "+ (server.method() == HTTP_GET) ? "GET" : "POST";
#   message += String("\nArguments: ") + server.args() + "\n";
#   for (uint8_t i = 0; i < server.args(); i++) {
#     message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
#   }
#   server.send(404, "text/plain", message);
# }


def handle_info(request: Request):
    data = {
        "temperature": microcontroller.cpu.temperature,
        "frequency": microcontroller.cpu.frequency,
        "voltage": microcontroller.cpu.voltage,
        "time": time.monotonic(),
        "avgDuration": running_average,
    }
    return JSONResponse(request, data)
