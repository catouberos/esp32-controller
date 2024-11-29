#include "configuration.hpp"
#include "esp_wifi_types.h"
#include <WiFi.h>

void init_wifi(Configuration config)
{
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(config.wifi.ssid, config.wifi.password);

  IPAddress IP = WiFi.softAPIP();
  Serial.printf("Started Access Point. Listen IP address: %s", IP);
}
