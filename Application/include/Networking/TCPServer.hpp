#pragma once

#include <cstdint>
#include <functional>
#include <Networking/Packet.hpp>
#include <Networking/TCPSocket.hpp>
#include <unordered_map>

namespace ag
{
class TCPServer
{
public:
  std::function<void(int client_ID)>                       on_client_connected;
  std::function<void(int client_ID)>                       on_client_disconnected;
  std::function<void(int client_ID, const Packet& packet)> on_packet_received;

  bool start(uint16_t port);
  void update();

  void send_to(int client_ID, const Packet& packet);
  void boardcast(const Packet& packet);
  void disconnect_client(int client_id);
  void stop();

private:
  TCPSocket                           m_listen_socket;
  std::unordered_map<int, TCPSocket*> m_clients;
  int                                 m_next_ID;

  void accept_new_clients();
  void receive_from_clients();
};
}  // namespace ag
