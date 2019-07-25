#include "wifi.hpp"
#include "udp_server.hpp"
//#include "ota.h"
#include "ws2812_controller.hpp"

#include <string>
#include <iostream>

extern "C" {
# include "esp_system.h"
# include "esp_log.h"
# include "nvs_flash.h"
}
const char* TAG = "main";

extern "C" void app_main() {
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ws2812_controller<1, GPIO_NUM_18> leds;

  uint8_t value = 0;
  while (true) {
    leds.write(0, value, value, value);
    vTaskDelay(500/portTICK_PERIOD_MS);
    ++value;
  }
}
