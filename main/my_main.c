/**
 * simple ESP-udp server example.
 * components udp and wifi capsulate wifi connection process and server setup.
 */

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "udp.h"
#include "ota.h"
#include "/home/boss/Arduino/libraries/creds/c.h"
#include "ws2812_control.h"

#define NUM_LEDS (1)
#define BLACK   0x00000000

const char* TAG = "main";

void app_main() {
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ws2812_control_init();
  struct led_state new_state;
  new_state.leds[0] = BLACK;
  uint8_t val = 0;

  while (true) {
    new_state.leds[0] = (val) | (val << 8) | (val << 16);
    val++;
    ws2812_write_leds(new_state);
  }
}
