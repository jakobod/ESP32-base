#include "wifi.hpp"
#include "udp_server.hpp"
#include "ota.h"
#include "/home/boss/Arduino/libraries/creds/c.h"
#include "ws2812_control.h"

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

  wifi::connect("FAUST-WLAN", "FAUST123");
  udp_server udp(8888);

  std::vector<uint8_t> buffer(2048);
  ESP_LOGI(TAG, "Waiting for incoming data");
  while (true) {
    auto recv = udp.receive(buffer, 1500);
    std::string msg(buffer.begin(), buffer.begin()+recv);
    std::cout << "received " << recv << " bytes: \"" << msg << "\"" << std::endl;
  }
}
