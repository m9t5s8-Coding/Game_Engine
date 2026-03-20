#pragma once

#include <cstdint>
#include <string>

namespace ag
{
class TCPSocket
{
public:
  TCPSocket();
  explicit TCPSocket(int existingFD);

  bool bind(uint16_t port);
  bool listen(int backlog = 5);
  bool connect(const std::string& ip, uint16_t port);

  int accept();

  bool send(const void* data, int size);
  int  receive(void* buffer, int max_size);

  void set_non_blocking(bool enabled = true);
  void close();

  bool is_valid() const
  {
    return m_FD >= 0;
  }
  int get_FDF() const
  {
    return m_FD;
  }

private:
  int m_FD;
};
}  // namespace ag
