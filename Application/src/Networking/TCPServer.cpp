#include <Core/Core.hpp>
#include <Networking/TCPServer.hpp>

namespace ag
{
bool TCPServer::start(uint16_t port)
{
  if (!m_listen_socket.bind(port))
    return false;
  if (!m_listen_socket.listen())
    return false;

  m_listen_socket.set_non_blocking(true);

  AERO_CORE_INFO("[TCPServer] Started on Port: {0}", port);
  return true;
}
void TCPServer::update()
{
  accept_new_clients();
  receive_from_clients();
}

void TCPServer::accept_new_clients()
{
  while (true)
  {
    int client_FD = m_listen_socket.accept();
    if (client_FD < 0)
      break;

    int        id            = m_next_ID++;
    TCPSocket* client_socket = new TCPSocket(client_FD);
    client_socket->set_non_blocking(true);
    m_clients[id] = client_socket;

    AERO_CORE_INFO("[TCPServer] Client Connected with ID: {0}", id);

    if (on_client_connected)
      on_client_connected(id);
  }
}

void TCPServer::receive_from_clients()
{
  std::vector<int> disconnected;

  for (auto& [id, socket] : m_clients)
  {
    uint32_t data_size    = 0;
    int      header_bytes = socket->receive((uint8_t*)&data_size, sizeof(uint32_t));

    if (header_bytes == 0)
    {
      disconnected.push_back(id);
      continue;
    }

    if (header_bytes < 0)
      continue;

    uint8_t type = 0;
    socket->receive(&type, 1);

    std::vector<uint8_t> buffer(data_size);

    int bytes = 0;
    int total = 0;

    while (total < (int)data_size)
    {
      bytes = socket->receive(buffer.data() + total, data_size - total);
      if (bytes <= 0)
        break;
      total += bytes;
    }

    Packet packet;
    if (Packet::deserialize(buffer.data(), total, packet, type))
    {
      if (on_packet_received)
        on_packet_received(id, packet);
    }
  }
  for (int dis_id : disconnected)
  {
    AERO_CORE_INFO("[TCPServer] Client {0} disconnected", dis_id);
    if (on_client_disconnected)
      on_client_disconnected(dis_id);

    delete m_clients[dis_id];
    m_clients.erase(dis_id);
  }
}

void TCPServer::send_to(int client_ID, const Packet& packet)
{
  auto it = m_clients.find(client_ID);
  if (it == m_clients.end())
  {
    AERO_CORE_ERROR("[TCPServer] Client {0} not found!", client_ID);
    return;
  }
  auto raw = packet.serialize();
  it->second->send(raw.data(), (int)raw.size());
}
void TCPServer::boardcast(const Packet& packet)
{
  auto raw = packet.serialize();
  for (auto& [id, socket] : m_clients)
  {
    socket->send(raw.data(), (int)raw.size());
  }
}
void TCPServer::disconnect_client(int client_ID)
{
  auto it = m_clients.find(client_ID);
  if (it == m_clients.end())
    return;

  it->second->close();
  delete it->second;
  m_clients.erase(it);

  if (on_client_disconnected)
    on_client_disconnected(client_ID);
}
void TCPServer::stop()
{
  for (auto& [id, socket] : m_clients)
  {
    socket->close();
    delete socket;
  }
  m_clients.clear();
  m_listen_socket.close();
  AERO_CORE_INFO("[TCPServer] Stopped Server");
}
}  // namespace ag
