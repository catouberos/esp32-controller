#include "server.hpp"

#include "pid.hpp"

#define MANUAL_MODE 0
#define CIRCLE_MODE 1
#define FORWARD_MODE 2
#define BACKWARD_MODE 3
#define RIGHT_MODE 4
#define LEFT_MODE 5
#define DIAGONAL_MODE 6
#define CIRCULAR_FIXED_DIRECTION_MODE 7
#define CIRCULAR_RADIAL_MODE 8
#define SQUARE_MODE 9

double speed_tl_ref = 0, speed_tr_ref = 0, speed_bl_ref = 0, speed_br_ref = 0;
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
  float cpr = 330;                // Counts per revolution of the encoder
  float wheel_radius = 0.0485;    // Radius of the wheel in meters
  float sampling_interval = 0.1;  // Sampling interval in seconds
  float max_rpm = 150.0;          // Maximum RPM for motors

  auto clamp = [](float value, float min_value, float max_value) {
    if (value > max_value) return max_value;
    if (value < min_value) return min_value;
    return value;
  };

  if (mode == CIRCLE_MODE) {
    float desired_radius = 0.5;  // Desired radius of the circle (meters)
    int encoder_counts = 330;    // Example encoder counts

    // Calculate linear velocity (v_y) in RPM
    float v_y_rpm = (encoder_counts / cpr) * (60 / sampling_interval);

    // Scale to max RPM if needed
    v_y_rpm = clamp(v_y_rpm, -max_rpm, max_rpm);

    // Calculate angular velocity (omega_z) in RPM
    float omega_z_rpm = v_y_rpm / desired_radius;

    // No sideways motion
    float v_x_rpm = 0.0;

    // Wheel speed calculations in RPM
    speed_tl_ref = v_y_rpm + v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_tr_ref = v_y_rpm - v_x_rpm + (lx + ly) * omega_z_rpm;
    speed_bl_ref = v_y_rpm - v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_br_ref = v_y_rpm + v_x_rpm + (lx + ly) * omega_z_rpm;

    // Clamp speeds to maximum RPM
    speed_tl_ref = clamp(speed_tl_ref, -max_rpm, max_rpm);
    speed_tr_ref = clamp(speed_tr_ref, -max_rpm, max_rpm);
    speed_bl_ref = clamp(speed_bl_ref, -max_rpm, max_rpm);
    speed_br_ref = clamp(speed_br_ref, -max_rpm, max_rpm);
  }

  if (mode == FORWARD_MODE) {
    float v_x_rpm = 0.0;      // No sideways motion
    float v_y_rpm = max_rpm;  // Forward motion in RPM
    float omega_z_rpm = 0.0;  // No rotation

    // Wheel speed calculations in RPM
    speed_tl_ref = v_y_rpm + v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_tr_ref = v_y_rpm - v_x_rpm + (lx + ly) * omega_z_rpm;
    speed_bl_ref = v_y_rpm - v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_br_ref = v_y_rpm + v_x_rpm + (lx + ly) * omega_z_rpm;
  }

  if (mode == BACKWARD_MODE) {
    float v_x_rpm = 0.0;       // No sideways motion
    float v_y_rpm = -max_rpm;  // Backward motion in RPM
    float omega_z_rpm = 0.0;   // No rotation

    // Wheel speed calculations in RPM
    speed_tl_ref = v_y_rpm + v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_tr_ref = v_y_rpm - v_x_rpm + (lx + ly) * omega_z_rpm;
    speed_bl_ref = v_y_rpm - v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_br_ref = v_y_rpm + v_x_rpm + (lx + ly) * omega_z_rpm;
  }

  if (mode == RIGHT_MODE) {
    float v_x_rpm = max_rpm;  // Rightward motion in RPM
    float v_y_rpm = 0.0;      // No forward/backward motion
    float omega_z_rpm = 0.0;  // No rotation

    // Wheel speed calculations in RPM
    speed_tl_ref = v_y_rpm + v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_tr_ref = v_y_rpm - v_x_rpm + (lx + ly) * omega_z_rpm;
    speed_bl_ref = v_y_rpm - v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_br_ref = v_y_rpm + v_x_rpm + (lx + ly) * omega_z_rpm;
  }

  if (mode == LEFT_MODE) {
    float v_x_rpm = -max_rpm;  // Leftward motion in RPM
    float v_y_rpm = 0.0;       // No forward/backward motion
    float omega_z_rpm = 0.0;   // No rotation

    // Wheel speed calculations in RPM
    speed_tl_ref = v_y_rpm + v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_tr_ref = v_y_rpm - v_x_rpm + (lx + ly) * omega_z_rpm;
    speed_bl_ref = v_y_rpm - v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_br_ref = v_y_rpm + v_x_rpm + (lx + ly) * omega_z_rpm;
  }

  if (mode == DIAGONAL_MODE) {
    float v_x_rpm = max_rpm;  // Leftward motion in RPM
    float v_y_rpm = max_rpm;  // No forward/backward motion
    float omega_z_rpm = 0.0;  // No rotation

    // Wheel speed calculations in RPM
    speed_tl_ref = v_y_rpm + v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_tr_ref = v_y_rpm - v_x_rpm + (lx + ly) * omega_z_rpm;
    speed_bl_ref = v_y_rpm - v_x_rpm - (lx + ly) * omega_z_rpm;
    speed_br_ref = v_y_rpm + v_x_rpm + (lx + ly) * omega_z_rpm;
  }

  if (mode == CIRCULAR_FIXED_DIRECTION_MODE) {
    // Circular motion without turning
    float desired_radius = 1.0;  // Radius of the circle (meters)
    float angular_speed = 1.0;   // Angular velocity (rad/s)

    // Linear velocity
    float v_x = -desired_radius * angular_speed;  // X-direction velocity
    float v_y = 0.0;                              // Y-direction velocity
    float omega_z = angular_speed;                // Rotational velocity

    // Calculate wheel speeds in RPM
    speed_tl_ref = (1 / wheel_radius) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / wheel_radius) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / wheel_radius) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / wheel_radius) * (v_y + v_x + (lx + ly) * omega_z);

    // Clamp speeds to max RPM
    speed_tl_ref = clamp(speed_tl_ref, -max_rpm, max_rpm);
    speed_tr_ref = clamp(speed_tr_ref, -max_rpm, max_rpm);
    speed_bl_ref = clamp(speed_bl_ref, -max_rpm, max_rpm);
    speed_br_ref = clamp(speed_br_ref, -max_rpm, max_rpm);
  }

  if (mode == CIRCULAR_RADIAL_MODE) {
    // Circular motion pointing away from the center
    float radius = 1.0;         // Radius of the circle (meters)
    float angular_speed = 1.0;  // Angular velocity (rad/s)

    // Linear velocity components
    float v_x = radius * angular_speed * sin(millis() / 1000.0);
    float v_y = radius * angular_speed * cos(millis() / 1000.0);
    float omega_z = 0.0;  // No rotation (fixed direction)

    // Calculate wheel speeds in RPM
    speed_tl_ref = (1 / wheel_radius) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / wheel_radius) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / wheel_radius) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / wheel_radius) * (v_y + v_x + (lx + ly) * omega_z);

    // Clamp speeds to max RPM
    speed_tl_ref = clamp(speed_tl_ref, -max_rpm, max_rpm);
    speed_tr_ref = clamp(speed_tr_ref, -max_rpm, max_rpm);
    speed_bl_ref = clamp(speed_bl_ref, -max_rpm, max_rpm);
    speed_br_ref = clamp(speed_br_ref, -max_rpm, max_rpm);
  }

  if (mode == SQUARE_MODE) {
    // Square motion
    static unsigned long last_time = 0;
    unsigned long current_time = millis();
    float delta_t =
        (current_time - last_time) / 1000.0;  // Time step in seconds
    last_time = current_time;

    float linear_speed = max_rpm;  // Linear speed in RPM
    float side_length = 2.0;       // Side length of the square (meters)
    float time_per_side =
        side_length / (linear_speed * wheel_radius * 2 * 3.14159 / 60.0);
    static int side = 0;  // Current side (0 to 3)
    static float elapsed_time = 0.0;

    // Update elapsed time
    elapsed_time += delta_t;

    // Determine velocity components
    float v_x = 0.0, v_y = 0.0;
    if (side == 0) {
      v_x = linear_speed;  // Move right
      v_y = 0.0;
    } else if (side == 1) {
      v_x = 0.0;
      v_y = linear_speed;  // Move up
    } else if (side == 2) {
      v_x = -linear_speed;  // Move left
      v_y = 0.0;
    } else if (side == 3) {
      v_x = 0.0;
      v_y = -linear_speed;  // Move down
    }

    // Check if time to switch side
    if (elapsed_time >= time_per_side) {
      side = (side + 1) % 4;  // Move to next side
      elapsed_time = 0.0;
    }

    // No rotational velocity
    float omega_z = 0.0;

    // Calculate wheel speeds in RPM
    speed_tl_ref = (1 / wheel_radius) * (v_y + v_x - (lx + ly) * omega_z);
    speed_tr_ref = (1 / wheel_radius) * (v_y - v_x + (lx + ly) * omega_z);
    speed_bl_ref = (1 / wheel_radius) * (v_y - v_x - (lx + ly) * omega_z);
    speed_br_ref = (1 / wheel_radius) * (v_y + v_x + (lx + ly) * omega_z);

    // Clamp speeds to max RPM
    speed_tl_ref = clamp(speed_tl_ref, -max_rpm, max_rpm);
    speed_tr_ref = clamp(speed_tr_ref, -max_rpm, max_rpm);
    speed_bl_ref = clamp(speed_bl_ref, -max_rpm, max_rpm);
    speed_br_ref = clamp(speed_br_ref, -max_rpm, max_rpm);
  }

  // Debugging output
  Serial.print("tl_ref (RPM): ");
  Serial.println(speed_tl_ref);
  Serial.print("tr_ref (RPM): ");
  Serial.println(speed_tr_ref);
  Serial.print("bl_ref (RPM): ");
  Serial.println(speed_bl_ref);
  Serial.print("br_ref (RPM): ");
  Serial.println(speed_br_ref);
}

float encoderToVelocityKmh(int encoderCounts, int cpr, float samplingInterval,
                           float wheelRadius) {
  // Calculate revolutions (counts divided by counts per revolution)
  float revolutions = static_cast<float>(encoderCounts) / cpr;

  // Calculate RPM (revolutions per minute)
  float rpm = revolutions * (60.0 / samplingInterval);

  // Convert RPM to angular velocity (rad/s)
  float angularVelocity =
      (rpm * 2.0 * 3.14159) / 60.0;  // Approximate π as 3.14159

  // Calculate linear velocity (m/s)
  float linearVelocityMps = angularVelocity * wheelRadius;

  // Convert linear velocity to km/h
  float velocityKmh = linearVelocityMps * 3.6;

  return velocityKmh;
}

int velocityToEncoderCountsKmh(float velocityKmh, float wheelRadius, int cpr,
                               float samplingInterval) {
  // Convert velocity from km/h to m/s
  float linearVelocityMps = velocityKmh / 3.6;

  // Calculate angular velocity (rad/s)
  float angularVelocity = linearVelocityMps / wheelRadius;

  // Convert angular velocity to RPM (revolutions per minute)
  float rpm =
      (angularVelocity * 60.0) / (2.0 * 3.14159);  // Approximate π as 3.14159

  // Calculate revolutions during the sampling interval
  float revolutions = rpm * (samplingInterval / 60.0);

  // Convert revolutions to encoder counts
  int encoderCounts =
      static_cast<int>(revolutions * cpr + 0.5);  // Round to nearest integer

  return encoderCounts;
}
