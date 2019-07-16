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

const char* TAG = "main";

void app_main() {
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  wifi_connect(SSID, PASS);
  printf("wifi connected\n");

  esp_err_t err = udp_server_init(8888);
  printf("udp_server initialized\n");

  ota_init();

  char* buf = (char*) malloc(2048);
  if (!buf) {
    ESP_LOGE(TAG, "buf could not be allocated");
  }
  printf("sizeof buf = %d", sizeof(buf));
  while (true) {
    udp_server_receive(buf, 2048);
    printf(buf);
    printf("\n");
    fflush(stdout);
  }
}
