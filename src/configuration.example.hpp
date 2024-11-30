#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cstdint>
class Configuration {
 public:
  struct WifiConfiguration {
    char *ssid = "SSID";
    char *password = "PASSWORD";
    char *mdns = "MDNS";
    uint8_t ip[4] = {192, 168, 1, 120};
    uint8_t gateway[4] = {192, 168, 1, 1};
    uint8_t subnet[4] = {255, 255, 255, 0};
  } wifi;

  static Configuration load();
};

#endif
