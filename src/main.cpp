#include <Arduino.h>

#include "configuration.hpp"
#include "pid.hpp"
#include "serial.hpp"
#include "server.hpp"
#include "wifi.hpp"

void setup() {
  Serial.begin(115200);
  Configuration config = Configuration::load();

  init_motors();
  init_encoders();
  init_pid();

  Serial.println("Initiating WiFi connection...");
  esp_err_t err = init_wifi(config);
  if (err == ESP_FAIL) {
    Serial.printf("WiFi failed to init, error: 0x%x\n", err);
  }

  Serial.println("Initiating server...");
  err = init_server();
  if (err == ESP_FAIL) {
    Serial.printf("Server failed to init, error: 0x%x\n", err);
  }
}

void loop() {
  check_connection();
  ws_cleanup();
  update_motion();
  update_speeds();
  compute_pid();
  run_motors();
  serial_print_write();
}
