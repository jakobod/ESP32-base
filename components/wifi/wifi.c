#include "wifi.h"
#include "string.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_log.h"

// defines
#define MAXIMUM_RETRY (5)

// constants
static const char *TAG = "wifi station";

// FreeRTOS event group to signal when we are connected
static EventGroupHandle_t s_wifi_event_group;
/* The event group allows multiple bits for each event, but we only care about
 * one event 
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

// variables
static int s_retry_num = 0;
static bool connected = false;

static esp_err_t event_handler(void *ctx, system_event_t *event) {
  switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      ESP_LOGI(TAG, "got ip:%s",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
      s_retry_num = 0;
      xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
      connected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      {
          if (s_retry_num < MAXIMUM_RETRY) {
              esp_wifi_connect();
              xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
              s_retry_num++;
              ESP_LOGI(TAG,"retry to connect to the AP");
          }
          ESP_LOGI(TAG,"connect to the AP fail\n");
          break;
      }
    default:
      break;
  }

  if (s_retry_num >= MAXIMUM_RETRY) 
    return ESP_ERR_WIFI_NOT_CONNECT;
  else 
    return ESP_OK;
}

void wifi_connect(const char* ssid, const char* password) {
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
    memcpy(wifi_config.sta.password, password, strlen(password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    while (!connected);
    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
             ssid, password);
}