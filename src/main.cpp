#include <Arduino.h>
#include "configuration.hpp"
#include "wifi.hpp"
#include "server.hpp"

void setup()
{
  Configuration config = Configuration::load();

  // init wifi AP
  init_wifi(config);
  Serial.println("Wifi configured!");

  init_server();
  Serial.println("Server configured!");
}

void loop()
{
  // put your main code here, to run repeatedly:
}
