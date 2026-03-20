#pragma once
#include <Core/Core.hpp>
#include <Networking/TCPClient.hpp>

#ifdef AERO_SERVER
  #include <Networking/TCPServer.hpp>
#endif

namespace ag
{
class NetworkManager
{
public:
  static TCPClient& get_client()
  {
    AERO_CORE_ASSERT(s_client, "NetworkManager Client Not set");
    return *s_client;
  }
  static void set_client(TCPClient* client)
  {
    s_client = client;
  }

#ifdef AERO_SERVER
  static TCPServer& get_server()
  {
    return *s_server;
  }
  static void set_server(TCPServer* server)
  {
    s_server = server;
  }
#endif

private:
  inline static TCPClient* s_client = nullptr;
#ifdef AERO_SERVER
  inline static TCPServer* s_server = nullptr;
#endif
};
}  // namespace ag
