#include <Arduino.h>

#include "configuration.hpp"
#include "server.hpp"
#include "wifi.hpp"

const int RPWM = 23;
const int LPWM = 22;

void setup() {
  Serial.begin(115200);
  Configuration config = Configuration::load();

  // init wifi AP
  init_wifi(config);
  Serial.println("Wifi configured!");

  init_server();
  Serial.println("Server configured!");

  // Set motor connections as outputs
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);

  // Stop motors
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
}

void loop() { ws_cleanup(); }
