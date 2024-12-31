#include "server.hpp"

double speed_tl_ref = 0, speed_tr_ref = 0, speed_bl_ref = 0, speed_br_ref = 0;
AsyncWebServer server(80);
AsyncWebSocket ws("/websocket");

esp_err_t init_server() {
  ws.onEvent(ws_event);
  server.addHandler(&ws);

  server.begin();

  return ESP_OK;
}

float r = 0.0485, lx = 0.12, ly = 0.09;

void ws_event(AsyncWebSocket *server, AsyncWebSocketClient *client,
              AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                    client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA: {
      AwsFrameInfo *info = (AwsFrameInfo *)arg;
      if (info->final && info->index == 0 && info->len == len &&
          info->opcode == WS_TEXT) {
        data[len] = 0;
        float x, y;
        char direction[50];

        if (sscanf((char *)data, "%f,%f,%49s", &x, &y, direction) == 3) {
          float v_x = abs(x) < 0.05 ? 0 : x * 10;
          float v_y = abs(y) < 0.05 ? 0 : y * 10;
          float omega_z = 0;
          Serial.print("x = ");
          Serial.println(x, 8);
          Serial.print("y = ");
          Serial.println(y, 8);
          Serial.print("direction = ");
          Serial.println(direction);
          speed_tl_ref = (1 / r) * (v_y + v_x - (lx + ly) * omega_z);
          speed_tr_ref = (1 / r) * (v_y - v_x + (lx + ly) * omega_z);
          speed_bl_ref = (1 / r) * (v_y - v_x - (lx + ly) * omega_z);
          speed_br_ref = (1 / r) * (v_y + v_x + (lx + ly) * omega_z);
          Serial.print("tl = ");
          Serial.println(speed_tl_ref, 8);
          Serial.print("tr = ");
          Serial.println(speed_tr_ref, 8);
          Serial.print("bl = ");
          Serial.println(speed_bl_ref, 8);
          Serial.print("br = ");
          Serial.println(speed_br_ref, 8);
        } else {
          Serial.println("Failed to parse data.");
        }

        // if data === toggle
        // if (strcmp((char *)data, "x") == 0) {
        //   ws.textAll(String(x));
        // }
      }
    } break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void ws_cleanup() { ws.cleanupClients(); }
