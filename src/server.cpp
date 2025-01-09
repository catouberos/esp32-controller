#include "server.hpp"

<<<<<<< HEAD
=======
#include "pid.hpp"

    >>>>>>> 009f89f
#define MANUAL_MODE 0
#define CIRCLE_MODE 1
#define FORWARD_MODE 2
#define BACKWARD_MODE 3
#define RIGHT_MODE 4
#define LEFT_MODE 5
#define DIAGONAL_MODE 6

    double speed_tl_ref = 0,
           speed_tr_ref = 0, speed_bl_ref = 0, speed_br_ref = 0;
extern int mode;
AsyncWebServer server(80);
AsyncWebSocket ws("/websocket");

esp_err_t init_server() {
  ws.onEvent(ws_event);
  server.addHandler(&ws);

  server.begin();

  return ESP_OK;
}

// wheel
float r = 0.0485, lx = 0.12, ly = 0.09;

void ws_event(AsyncWebSocket *server, AsyncWebSocketClient *client,
              AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                    client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      speed_tl_ref = 0;
      speed_tr_ref = 0;
      speed_bl_ref = 0;
      speed_br_ref = 0;
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA: {
      AwsFrameInfo *info = (AwsFrameInfo *)arg;
      if (info->final && info->index == 0 && info->len == len &&
          info->opcode == WS_TEXT) {
        data[len] = 0;
        float x, y, omega;
        int mul, omega_mul;

        if (sscanf((char *)data, "%f,%f,%f,%d,%d", &x, &y, &omega, &mul,
                   &omega_mul) == 5) {
          float v_x = abs(x) < 0.05 ? 0 : x * mul;
          float v_y = abs(y) < 0.05 ? 0 : y * mul;
          float omega_z = omega * -omega_mul;

          speed_tl_ref = (1 / r) * (v_y + v_x - (lx + ly) * omega_z);
          speed_tr_ref = (1 / r) * (v_y - v_x + (lx + ly) * omega_z);
          speed_bl_ref = (1 / r) * (v_y - v_x - (lx + ly) * omega_z);
          speed_br_ref = (1 / r) * (v_y + v_x + (lx + ly) * omega_z);

        } else {
          Serial.println("Failed to parse data.");
        }

        ws.printfAll(
            "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
            mot_tl_cmd, mot_tr_cmd, mot_bl_cmd, mot_br_cmd, speed_tl, speed_tr,
            speed_bl, speed_br, speed_tl_ref, speed_tr_ref, speed_bl_ref,
            speed_br_ref);

        // if data === toggle
        // if (strcmp((char *)data, "x") == 0) {
        //   ws.textAll(String(x));
        // }
      }
    } break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      speed_tl_ref = 0;
      speed_tr_ref = 0;
      speed_bl_ref = 0;
      speed_br_ref = 0;
      break;
  }
}

void ws_cleanup() { ws.cleanupClients(); }

void update_motion() {
  if (mode == CIRCLE_MODE) {
    float v_x = 0.0;           // No sideways motion
    float v_y = 0.75 * 20;     // Forward motion
    float omega_z = 0.5 * 40;  // Rotation rate

    speed_tl_ref = (1 / r) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / r) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / r) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / r) * (v_y + v_x + (lx + ly) * omega_z);
  }

  if (mode == FORWARD_MODE) {
    float v_x = 0.0;       // No sideways motion
    float v_y = 1.0 * 20;  // Forward motion
    float omega_z = 0.0;   // No rotation

    speed_tl_ref = (1 / r) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / r) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / r) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / r) * (v_y + v_x + (lx + ly) * omega_z);
  }

  if (mode == BACKWARD_MODE) {
    float v_x = 0.0;        // No sideways motion
    float v_y = -1.0 * 20;  // Backward motion
    float omega_z = 0.0;    // No rotation

    speed_tl_ref = (1 / r) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / r) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / r) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / r) * (v_y + v_x + (lx + ly) * omega_z);
  }
  if (mode == RIGHT_MODE) {
    float v_x = 1.0 * 20;  // Rightward motion
    float v_y = 0.0;       // No forward/backward motion
    float omega_z = 0.0;   // No rotation

    speed_tl_ref = (1 / r) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / r) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / r) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / r) * (v_y + v_x + (lx + ly) * omega_z);
  }

  if (mode == LEFT_MODE) {
    float v_x = -1.0 * 20;  // Leftward motion
    float v_y = 0.0;        // No forward/backward motion
    float omega_z = 0.0;    // No rotation

    speed_tl_ref = (1 / r) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / r) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / r) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / r) * (v_y + v_x + (lx + ly) * omega_z);
  }

  if (mode == DIAGONAL_MODE) {
    float v_x = 1.0 * 14.14;  // Diagonal motion (45 degrees)
    float v_y = 1.0 * 14.14;  // Forward motion
    float omega_z = 0.0;      // No rotation

    speed_tl_ref = (1 / r) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / r) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / r) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / r) * (v_y + v_x + (lx + ly) * omega_z);
  }

  Serial.print("tl = ");
  Serial.println(speed_tl_ref, 8);
  Serial.print("tr = ");
  Serial.println(speed_tr_ref, 8);
  Serial.print("bl = ");
  Serial.println(speed_bl_ref, 8);
  Serial.print("br = ");
  Serial.println(speed_br_ref, 8);
}
