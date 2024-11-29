#include "server.hpp"

AsyncWebServer server(80);

void init_server() {
  // start SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.begin();
}
