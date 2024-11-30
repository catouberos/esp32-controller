#include <Arduino.h>

#include "configuration.hpp"
#include "server.hpp"
#include "wifi.hpp"

const int RPWM = 23;
const int LPWM = 22;

void setup() {
  Serial.begin(115200);
  Configuration config = Configuration::load();

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

  // Set motor connections as outputs
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);

  // Stop motors
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
}

void loop() {
  check_connection();
  ws_cleanup();
}
