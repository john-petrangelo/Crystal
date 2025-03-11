#pragma once

class HTTPServer;
class HTTPRequest;
class HTTPResponse;

class HTTPHandlers {
public:
  static void setup(HTTPServer &httpServer);

private:
  static HTTPResponse handleRoot(HTTPRequest const &request);
  static HTTPResponse handleCSS(HTTPRequest const &request);
  static HTTPResponse handleJS(HTTPRequest const &request);

  static HTTPResponse handleSetup(HTTPRequest const &request);
  static HTTPResponse handleConnect(HTTPRequest const &request);

  static HTTPResponse handleStatus(HTTPRequest const &request);
  static HTTPResponse handleGetWiFiNetworks(HTTPRequest const &request);

  static HTTPResponse handleGetBrightness(HTTPRequest const &reqeust);
  static HTTPResponse handleSetBrightness(HTTPRequest const &request);
  static HTTPResponse handleGetGamma(HTTPRequest const &reqeust);
  static HTTPResponse handleSetGamma(HTTPRequest const &request);

  static HTTPResponse handleCrystal(HTTPRequest const &request);
  static HTTPResponse handleFlame(HTTPRequest const &request);
  static HTTPResponse handleJacobsLadder(HTTPRequest const &request);
  static HTTPResponse handleRainbow(HTTPRequest const &request);
  static HTTPResponse handleSolid(HTTPRequest const &request);
  static HTTPResponse handleWarpCore(HTTPRequest const &request);
  static HTTPResponse handleDemo1(HTTPRequest const &request);
  static HTTPResponse handleDemo2(HTTPRequest const &request);
  static HTTPResponse handleDemo3(HTTPRequest const &request);
  static HTTPResponse handleGetData(HTTPRequest const &request);
};
