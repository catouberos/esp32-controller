#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class Configuration {
 public:
  struct WifiConfiguration {
    char *ssid = "Green Robot";
    char *password = "greenrobot";
  } wifi;

  static Configuration load();
};

#endif
