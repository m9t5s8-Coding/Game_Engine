#include <Networking/TCPClient.hpp>

namespace ag
{
bool TCPClient::connect(std::string& ip, uint16_t port)
{
  if (!m_socket.connect(ip, port))
    return false;

  m_socket.set_non_blocking(true);

  if (on_connected)
    on_connected();

  return true;
}
void TCPClient::update()
{
  if (!m_socket.is_valid())
    return;

  uint32_t data_size    = 0;
  int      header_bytes = m_socket.receive((uint8_t*)&data_size, sizeof(uint32_t));

  if (header_bytes == 0)
  {
    AERO_CORE_INFO("Server Disconnected");
    m_socket.close();
    if (on_disconnected)
      on_disconnected();
    return;
  }

  if (header_bytes < 0)
    return;

  uint8_t type = 0;
  m_socket.receive(&type, 1);

  std::vector<uint8_t> buffer(data_size);

  int bytes = 0;
  int total = 0;

  while (total < (int)data_size)
  {
    bytes = m_socket.receive(buffer.data() + total, data_size - total);
    if (bytes <= 0)
      break;
    total += bytes;
  }

  Packet packet;
  if (Packet::deserialize(buffer.data(), total, packet, type))
  {
    if (on_packet_received)
      on_packet_received(packet);
  }
}

bool TCPClient::send(const Packet& packet)
{
  if (!m_socket.is_valid())
  {
    AERO_CORE_ERROR("[TCPClient] Cannot send - not Connected");
    return false;
  }

  auto data = packet.serialize();

  int total = 0;
  int size  = data.size();

  while (total < size)
  {
    int sent = m_socket.send(data.data() + total, size - total);
    if (sent <= 0)
      return false;
    total += sent;
  }
  return true;
}
void TCPClient::disconnect()
{
  m_socket.close();
  if (on_disconnected)
    on_disconnected();
}
}  // namespace ag
