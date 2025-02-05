#pragma once

#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPResponse.h"

extern HTTPResponse handleRoot(HTTPRequest const &request);
extern HTTPResponse handleCSS(HTTPRequest const &request);
extern HTTPResponse handleJS(HTTPRequest const &request);

extern HTTPResponse handleStatus(HTTPRequest const &request);

extern HTTPResponse handleGetBrightness(HTTPRequest const &reqeust);
extern HTTPResponse handleSetBrightness(HTTPRequest const &request);
extern HTTPResponse handleGetGamma(HTTPRequest const &reqeust);
extern HTTPResponse handleSetGamma(HTTPRequest const &request);

extern HTTPResponse handleCrystal(HTTPRequest const &request);
extern HTTPResponse handleFlame(HTTPRequest const &request);
extern HTTPResponse handleJacobsLadder(HTTPRequest const &request);
extern HTTPResponse handleRainbow(HTTPRequest const &request);
extern HTTPResponse handleSolid(HTTPRequest const &request);
extern HTTPResponse handleWarpCore(HTTPRequest const &request);
extern HTTPResponse handleDemo1(HTTPRequest const &request);
extern HTTPResponse handleDemo2(HTTPRequest const &request);
extern HTTPResponse handleDemo3(HTTPRequest const &request);
extern HTTPResponse handleGetData(HTTPRequest const &request);
