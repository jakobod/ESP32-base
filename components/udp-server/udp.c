#include "udp.h"
#include <string.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "esp_log.h"
#include <lwip/netdb.h>

static const char* TAG = "UDP";
static int sock = -1;

esp_err_t udp_server_init(uint16_t port) {
  int addr_family = AF_INET;
  int ip_protocol = IPPROTO_IP;

  struct sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(port);

  sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
  if (sock < 0) {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "Socket created");

  int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err < 0) {
    ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    close(sock);
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "Socket bound, port %d", port);
  return ESP_OK;
}

esp_err_t udp_server_receive(char* rx_buffer, size_t size) {
  char addr_str[128];

  ESP_LOGI(TAG, "receiving data");
  struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
  socklen_t socklen = sizeof(source_addr);
  int len = recvfrom(sock, rx_buffer, size - 1, 0, 
                    (struct sockaddr *)&source_addr, &socklen);

  if (len < 0) {
    ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
    return ESP_FAIL;
  } else {
    // Get the sender's ip address as string
    if (source_addr.sin6_family == PF_INET) {
        inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
    } else if (source_addr.sin6_family == PF_INET6) {
        inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
    }

    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
    ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
  }
  return len;
}

ssize_t udp_server_send(char* tx_buffer, size_t size, const char* host, const uint16_t port) {
  struct sockaddr_in dest_addr;
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_addr.s_addr = inet_addr(host);
  dest_addr.sin_port = htons(port);

  ssize_t err = sendto(sock, tx_buffer, size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err < 0) {
      ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
  }
  return err;
}