#include <WiFi.h>

#include "IPAddress.h"
#include "configuration.hpp"
#include "esp_err.h"
#include "esp_wifi_types.h"

unsigned long previous_time = 0;
unsigned long reconnect_delay = 20000;

esp_err_t init_wifi(Configuration config) {
  WiFi.mode(WIFI_MODE_STA);

  IPAddress local_IP(config.wifi.ip);
  IPAddress gateway(config.wifi.gateway);
  IPAddress subnet(config.wifi.subnet);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    return ESP_FAIL;
  }

  WiFi.begin(config.wifi.ssid, config.wifi.password);

  return ESP_OK;
}

void check_connection() {
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long current_time = millis();
    unsigned long delta_time = current_time - previous_time;

    if (delta_time >= reconnect_delay) {
      WiFi.disconnect();
      WiFi.reconnect();
      previous_time = current_time;
    }
  }
}
