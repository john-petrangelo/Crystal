#pragma once

#include <ESP8266WebServer.h>

#include "Renderer.h"

extern String hostname;

// Server used for HTTP requests
extern ESP8266WebServer server;

extern void setupNetwork(Renderer *renderer);
extern void loopNetwork();
extern void loopLogger();
extern Renderer* getNetworkRenderer();
