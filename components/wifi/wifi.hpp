#ifndef WIFI_HPP
#define WIFI_HPP

#include <string>
#include <freertos/FreeRTOS.h>
#include "freertos/event_groups.h"
#include "esp_event_loop.h"

class wifi {
  static const char tag_[];
  static const uint8_t max_retries_;
  static const int CONNECTED_BIT;

  static EventGroupHandle_t event_group_;

  static esp_err_t event_handler(void *ctx, system_event_t *event);
public:

  static void connect(const std::string& ssid, const std::string& password);
};

#endif // WIFI_HPP