#ifndef UDP_H
#define UDP_H

#include "esp_err.h"

esp_err_t udp_server_init(uint16_t port);
esp_err_t udp_server_receive(char* rx_buffer, size_t size);
ssize_t udp_server_send(char* tx_buffer, size_t size, const char* host, const uint16_t port);

#endif // UDP_H