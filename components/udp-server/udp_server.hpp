#ifndef UDP_HPP
#define UDP_HPP

#include "esp_err.h"
#include <string>
#include <vector>

class udp_server {
  static const char tag_[];
  int socket_;

  void fail(const std::string& msg);
public:
  udp_server(uint16_t port);
  ~udp_server() = default;

  /**
   * receive data from the socket. BLOCKING CALL
   * @param buf vector to which the data should be written.
   *            will receive buf.size() amount of bytes
   */
  size_t receive(std::vector<uint8_t>& buf, size_t size);

  ssize_t send(std::vector<uint8_t>& buf, const std::string& host, 
               const uint16_t port);
};

#endif // UDP_HPP
