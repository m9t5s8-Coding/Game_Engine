#pragma once

#include <functional>
#include <Networking/Packet.hpp>
#include <Networking/TCPSocket.hpp>
#include <string>

namespace ag
{
class TCPClient
{
public:
  std::function<void()>              on_connected;
  std::function<void()>              on_disconnected;
  std::function<void(const Packet&)> on_packet_received;

  bool connect(std::string& ip, uint16_t port);
  void update();

  bool send(const Packet& packet);
  void disconnect();

  bool is_connected()
  {
    return m_socket.is_valid();
  }

private:
  TCPSocket m_socket;
};
}  // namespace ag
