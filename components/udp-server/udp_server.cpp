#include "udp_server.hpp"

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#include <cstring>
#include <string>
#include <stdexcept>
#include <esp_log.h>

const char udp_server::tag_[] = "UDP_SERVER";

udp_server::udp_server(uint16_t port) : socket_(-1) {
  sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(port);

  socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (socket_ < 0) {
    fail("Unable to create socket: errno " + errno);
  }

  auto err = bind(socket_, reinterpret_cast<sockaddr*>(&dest_addr),
                  sizeof(dest_addr));
  if (err < 0) {
    close(socket_);
    fail("Socket unable to bind: errno %d" + errno);
  }
  ESP_LOGI(tag_, "udp server successfully initialized");
}

size_t udp_server::receive(std::vector<uint8_t>& buf, size_t size) {
  sockaddr_in6 source_addr;
  socklen_t socklen = sizeof(source_addr);
  auto len = recvfrom(socket_, buf.data(), size, 0,
                      reinterpret_cast<sockaddr*>(&source_addr), &socklen);

  if (len < 0) throw std::runtime_error(std::string(tag_) + "recvfrom failed");
  return len;
}

ssize_t udp_server::send(std::vector<uint8_t>& buf, const std::string& host, 
                         const uint16_t port) {
  sockaddr_in dest_addr;
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_addr.s_addr = inet_addr(host.c_str());
  dest_addr.sin_port = htons(port);

  auto err = sendto(socket_, buf.data(), buf.size(), 0, 
                       reinterpret_cast<sockaddr*>(&dest_addr), sizeof(dest_addr));
  if (err < 0) {
      fail("Error occurred during sending: errno %d" + errno);
  }
  return err;
}

void udp_server::fail(const std::string& msg) {
  throw std::runtime_error(std::string(tag_) + msg);
}
