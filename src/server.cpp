#include "server.hpp"

AsyncWebServer server(80);

void init_server()
{
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    server.begin();
}
