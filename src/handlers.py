import time

import microcontroller
from adafruit_httpserver import Request, JSONResponse, FileResponse, Server, Route, GET, PUT, Response
from luminaria.renderer.circuitpython_neopixel_renderer import Renderer

pixels: Renderer

running_average = 0.0


def report_render_time(render_time):
    global running_average
    running_average = running_average * 0.9 + render_time * 0.1


def set_renderer(pixels_in):
    global renderer
    renderer = pixels_in


def setup_handlers(server: Server):
    server.add_routes([
        Route("/", GET, handle_root),
        Route("/crystal.css", GET, handle_css),
        Route("/crystal.js", GET, handle_js),
        Route("/brightness", GET, handle_get_brightness),
        Route("/brightness", PUT, handle_set_brightness),
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
        "value": pixels.brightness,
    })


def handle_set_brightness(request: Request):
    value = request.query_params["value"]
    if not value:
        return

    # Convert from string to float
    float_value = float(value)

    # Apply gamma correction
    float_value = float_value * float_value

    pixels.brightness = float_value

    return Response(request)


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
#
# void handleCrystal() {
#   StaticJsonDocument<384> doc;
#   DeserializationError error = deserializeJson(doc, server.arg("plain"));
#   if (error) {
#     Logger::logf("handleCrystal failed to parse JSON: %s\n", error.c_str());
#     server.send(400, "text/plain", error.c_str());
#     return;
#   }
#
#   Color upperColor = strtol(doc["upper"]["color"], 0, 16);
#   Color middleColor = strtol(doc["middle"]["color"], 0, 16);
#   Color lowerColor = strtol(doc["lower"]["color"], 0, 16);
#   float upperSpeed = doc["upper"]["speed"];
#   float middleSpeed = doc["middle"]["speed"];
#   float lowerSpeed = doc["lower"]["speed"];
#
#   float upperPeriodSec = fmap(upperSpeed, 0.0, 1.0, 11.0, 1.0);
#   float middlePeriodSec = fmap(middleSpeed, 0.0, 1.0, 11.0, 1.0);
#   float lowerPeriodSec = fmap(lowerSpeed, 0.0, 1.0, 11.0, 1.0);
#
#   renderer.setModel(makeCrystal(
#     upperColor, upperPeriodSec,
#     middleColor, middlePeriodSec,
#     lowerColor, lowerPeriodSec));
#   server.send(200, "text/plain", "");
# }
#
# void handleFlame() {
#   renderer.setModel(std::make_shared<Flame>());
#   server.send(200, "text/plain", "");
# }
#
# void handleRainbow() {
#   StaticJsonDocument<128> doc;
#   DeserializationError error = deserializeJson(doc, server.arg("plain"));
#   if (error) {
#     Logger::logf("handleRainbow failed to parse JSON: %s\n", error.c_str());
#     server.send(400, "text/plain", error.c_str());
#     return;
#   }
#
#   String mode = doc["mode"];
#   float speed = doc["speed"];
#
#   std::shared_ptr<Model> gm = nullptr;
#   if (mode == "classic") {
#     gm = std::make_shared<MultiGradientModel>("vivid-gradient",
#       8, RED, VIOLET, INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED);
#   } else if (mode == "tv") {
#     gm = std::make_shared<MultiGradientModel>("tv-gradient",
#       8, 0x0000BF, 0xBF0000, 0xBF00C0, 0x00BF00, 0x00BFBE, 0xBFBF00, 0xBFBFBF, 0x666666);
#   } else if (mode == "50s") {
#     gm = std::make_shared<MultiGradientModel>("50s-gradient",
#       6, 0xB26463, 0x79C874, 0x8AC5BD, 0x9A942E, 0x937535, 0xB26463);
#   } else if (mode == "60s") {
#     gm = std::make_shared<MultiGradientModel>("60s-gradient",
#       6, 0xFCD53A, 0xBFC847, 0xF7790B, 0xDC4E80, 0x7BD0EC, 0xFCD53A);
#   } else if (mode == "70s") {
#     gm = std::make_shared<MultiGradientModel>("70s-gradient",
#       6, 0xE59F23, 0x925E18, 0x758C32, 0xCE985A, 0xE15623, 0xE59F23);
#   } else if (mode == "80s") {
#     gm = std::make_shared<MultiGradientModel>("80s-gradient",
#       6, 0xFE167A, 0x003BB1, 0x00C8AB, 0xFFC800, 0x8000E2, 0xFE167A);
#   } else if (mode == "90s") {
#     gm = std::make_shared<MultiGradientModel>("90s-gradient",
#       6, 0xB03A50, 0x7F5F14, 0x065B44, 0x4B183C, 0x6B0A29, 0xB03A50);
#   } else if (mode == "00s") {
#     gm = std::make_shared<MultiGradientModel>("00s-gradient",
#       6, 0x79AED6, 0x8F8C84, 0xC4A051, 0x69322C, 0x3D4D4C, 0x79AED6);
#   } else if (mode == "10s") {
#     gm = std::make_shared<MultiGradientModel>("10s-gradient",
#       6, 0xF0A2A3, 0x62656D, 0x8E6E29, 0x725A47, 0x4F7018, 0xF0A2A3);
#   } else {
#     // We shouldn't get here, but assume "classic" as a default in case we do.
#     gm = std::make_shared<MultiGradientModel>("rainbow-gradient",
#       8, RED, VIOLET, INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED);
#   }
#
#   auto model = renderer.getModel();
#   if (model->getName() == "rainbow-rotate") {
#     auto rainbowModel = static_cast<Rotate*>(model.get());
#     if (rainbowModel != NULL) {
#       rainbowModel->setSpeed(speed);
#       rainbowModel->setModel(gm);
#       server.send(200, "text/plain", "");
#       return;
#     }
#   }
#
#   auto rm = std::make_shared<Rotate>("rainbow-rotate", speed, gm);
#   renderer.setModel(rm);
#
#   server.send(200, "text/plain", "");
# }
#
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


def handle_info(request: Request):
    data = {
        "temperature": microcontroller.cpu.temperature,
        "frequency": microcontroller.cpu.frequency,
        "voltage": microcontroller.cpu.voltage,
        "time": time.monotonic(),
        "avgDuration": running_average,
    }
    return JSONResponse(request, data)
