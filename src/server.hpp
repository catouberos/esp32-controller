#ifndef SERVER_H
#define SERVER_H

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

void init_server();
void ws_event(AsyncWebSocket *server, AsyncWebSocketClient *client,
              AwsEventType type, void *arg, uint8_t *data, size_t len);
void ws_cleanup();

#endif
