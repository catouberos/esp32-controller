#include <Arduino.h>

#include "configuration.hpp"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include "server.hpp"
#include "wifi.hpp"

const int RPWM = 17;
const int LPWM = 18;

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
  pinMode(4, INPUT);
  pinMode(16, INPUT);
}

void loop() {
  check_connection();
  ws_cleanup();
  int pin4 = digitalRead(4);
  int pin16 = digitalRead(16);
  if (pin4 == HIGH) {
    analogWrite(RPWM, 100);
  } else {
    analogWrite(RPWM, 0);
  }

  if (pin16 == HIGH) {
    analogWrite(LPWM, 100);
  } else {
    analogWrite(LPWM, 0);
  }
}
