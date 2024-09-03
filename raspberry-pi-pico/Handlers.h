#pragma once

#include "HTTP/HTTPServer.h"

extern HTTPResponse handleRoot(HTTPRequest const &request);
//extern void handleCSS();
//extern void handleJS();

extern HTTPResponse handleStatus(HTTPRequest const &request);

//extern void handleGetBrightness();
//extern void handleSetBrightness();

//extern void handleCrystal();
//extern void handleFlame();
//extern void handleJacobsLadder();
//extern void handleRainbow();
extern HTTPResponse handleSolid(HTTPRequest const &request);
//extern void handleWarpCore();
//extern void handleDemo1();
//extern void handleDemo2();
//extern void handleDemo3();
