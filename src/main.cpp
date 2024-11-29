#include <Arduino.h>

#include "configuration.hpp"
#include "server.hpp"
#include "wifi.hpp"

void setup() {
  Serial.begin(115200);
  Configuration config = Configuration::load();

  // init wifi AP
  init_wifi(config);
  Serial.println("Wifi configured!");

  init_server();
  Serial.println("Server configured!");
}

void loop() {
  // put your main code here, to run repeatedly:
}
