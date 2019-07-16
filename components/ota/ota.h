#ifndef OTA_H
#define OTA_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "/home/boss/esp/esp-idf/components/app_update/include/esp_ota_ops.h"
#include "/home/boss/esp/esp-idf/components/esp_http_client/include/esp_http_client.h"
#include "/home/boss/esp/esp-idf/components/bootloader_support/include/esp_flash_partitions.h"
#include "/home/boss/esp/esp-idf/components/spi_flash/include/esp_partition.h"

void ota_init(void);

#endif // OTA_H