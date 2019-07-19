#include "wifi.hpp"
#include "string.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <iostream>

// constants
const char wifi::tag_[] = "wifi station";
const uint8_t wifi::max_retries_ = 5;
const int wifi::CONNECTED_BIT = BIT0;

// FreeRTOS event group to signal when we are connected
EventGroupHandle_t wifi::event_group_;

// variables
static int s_retry_num = 0;
static bool connected = false;

esp_err_t wifi::event_handler(void *ctx, system_event_t *event) {
  switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      break;
    
    case SYSTEM_EVENT_STA_GOT_IP:
      ESP_LOGI(tag_, "got ip:%s",
               ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
      s_retry_num = 0;
      xEventGroupSetBits(event_group_, CONNECTED_BIT);
      break;
    
    case SYSTEM_EVENT_STA_DISCONNECTED:
      if (s_retry_num < max_retries_) {
          esp_wifi_connect();
          xEventGroupClearBits(event_group_, CONNECTED_BIT);
          ++s_retry_num;
          ESP_LOGI(tag_,"retry to connect to the AP");
      } else  {
        ESP_LOGI(tag_,"connect to the AP fail\n");
      }
      break;

    default:
      break;
  }

  return ESP_OK;
}

void wifi::connect(const std::string& ssid, const std::string& password) {
    event_group_ = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, ssid.c_str(), ssid.size());
    memcpy(wifi_config.sta.password, password.c_str(), password.size());

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    xEventGroupWaitBits(event_group_, CONNECTED_BIT, false, false, 5000/portTICK_PERIOD_MS);

    ESP_LOGI(tag_, "wifi_     else if( ( uxBits & BIT_4 ) != 0 )init_sta finished.");
}