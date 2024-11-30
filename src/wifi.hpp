#ifndef WIFI_H
#define WIFI_H

#include <WiFi.h>

#include "configuration.hpp"

esp_err_t init_wifi(Configuration config);
void check_connection();

#endif
