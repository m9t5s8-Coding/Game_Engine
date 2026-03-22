

#ifdef PLATFORM_ANDROID
  #include <arpa/inet.h>
  #include <asm-generic/socket.h>
  #include <fcntl.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <unistd.h>

  #include <Core/Core.hpp>
  #include <Networking/TCPSocket.hpp>

namespace ag
{
TCPSocket::TCPSocket()
  : m_FD(-1)
{
  m_FD = socket(AF_INET, SOCK_STREAM, 0);

  if (m_FD < 0)
  {
    std::cerr << "[TCPSocket] Failed to create socket\n";
  }
}

TCPSocket::TCPSocket(int existingFD)
  : m_FD(existingFD)
{
}

bool TCPSocket::bind(uint16_t port)
{
  int opt = 1;

  setsockopt(m_FD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port        = htons(port);

  if (::bind(m_FD, (sockaddr*)&addr, sizeof(addr)) < 0)
  {
    std::cerr << "[TCPSocket] Bind Failed on port: " << port << "\n";
    return false;
  }

  AERO_CORE_INFO("Bound to port: {0}", port);
  return true;
}
bool TCPSocket::listen(int backlog)
{
  if (::listen(m_FD, backlog) < 0)
  {
    std::cerr << "[TCPSocket] Listen Failed\n";
    return false;
  }
  std::cout << "[TCPSocket] Listening...\n";
  return true;
}
bool TCPSocket::connect(const std::string& ip, uint16_t port)
{
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port);

  if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0)
  {
    std::cerr << "[TCPSocket] Invalid IP address: " << ip << "\n";
    return false;
  }

  if (::connect(m_FD, (sockaddr*)&addr, sizeof(addr)) < 0)
  {
    std::cerr << "[TCPSocket] Connect failed to " << ip << ":" << port << "\n";
    return false;
  }

  std::cout << "[TCPSocket] Connected to " << ip << ":" << port << "\n";
  return true;
}

int TCPSocket::accept()
{
  sockaddr_in clientAddr{};
  socklen_t   len = sizeof(clientAddr);

  int clientFd = ::accept(m_FD, (sockaddr*)&clientAddr, &len);
  if (clientFd < 0)
  {
    return -1;
  }

  std::cout << "[TCPSocket] New client accepted: " << inet_ntoa(clientAddr.sin_addr) << ":"
            << ntohs(clientAddr.sin_port) << "\n";

  return clientFd;
}

bool TCPSocket::send(const void* data, int size)
{
  int sent = ::send(m_FD, data, size, MSG_NOSIGNAL);
  if (sent < 0)
  {
    std::cerr << "[TCPSocket] Send failed\n";
    return false;
  }
  return true;
}
int TCPSocket::receive(void* buffer, int max_size)
{
  int bytes = ::recv(m_FD, buffer, max_size, 0);

  if (bytes == 0)
  {
    std::cout << "[TCPSocket] Connection closed by peer\n";
    return 0;
  }

  if (bytes < 0)
  {
    return -1;
  }

  return bytes;
}

void TCPSocket::set_non_blocking(bool enabled)
{
  int flags = fcntl(m_FD, F_GETFL, 0);
  if (enabled)
    fcntl(m_FD, F_SETFL, flags | O_NONBLOCK);
  else
    fcntl(m_FD, F_SETFL, flags & ~O_NONBLOCK);
}
void TCPSocket::close()
{
  if (m_FD >= 0)
  {
    ::close(m_FD);
    m_FD = -1;
  }
}
}  // namespace ag

#endif
