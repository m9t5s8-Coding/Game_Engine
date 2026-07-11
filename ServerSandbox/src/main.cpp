#include <Aero.hpp>
#include <chrono>
#include <csignal>
#include <thread>

static bool s_running = true;

void on_signal(int) { s_running = false; }

int main(int argc, char** argv) {
  ag::Log::init_simple();

  if (argc < 2) {
    AERO_SERVER_ERROR("Usuage Server <project_path>");
    return -1;
  }

  std::string project_path = argv[1];
  AERO_SERVER_INFO("Loading Project: {0}", project_path);

  auto project = ag::Project::load_project(project_path);
  if (!project) {
    AERO_SERVER_ERROR("Failed to Load Project!");
    return -1;
  }

  auto& config = project->get_server_config();

  if (!config.enabled) {
    AERO_CORE_ERROR("Networking not enabled in project!");
    return -1;
  }

  AERO_SERVER_INFO("Port: {0}", config.port);
  AERO_SERVER_INFO("Max Clients: {0}", config.max_clients);
  AERO_SERVER_INFO("Tick Rate: {0}", config.tick_rate);
  AERO_SERVER_INFO("Scripts: {0}", config.scripts.size());

  for (auto& script : config.scripts) {
    std::string full_path = project->get_directory() + project->get_scripts_directory() + script;
    AERO_SERVER_INFO("Loading script: {0}", full_path);
    ag::ScriptManager::load_script(full_path);
  }

  ag::TCPServer server;
  ag::NetworkManager::set_server(&server);

  server.on_client_connected = [](int id) {
    ag::ScriptManager::call("on_client_connected", id);
  };

  server.on_client_disconnected = [](int id) {
    ag::ScriptManager::call("on_client_disconnected", id);
  };

  server.on_packet_received = [&](int id, const ag::Packet& packet) {
    ag::ScriptManager::call("on_packet_received", id, packet);
  };

  if (!server.start(config.port)) {
    AERO_SERVER_ERROR("Failed to Start!\n");
    return -1;
  }

  ag::ScriptManager::call("on_server_start");
  AERO_SERVER_INFO("Server Running on Port: {0}", config.port);

  const auto tick      = std::chrono::milliseconds(1000 / config.tick_rate);
  auto       last_time = std::chrono::steady_clock::now();

  while (s_running) {
    auto  now = std::chrono::steady_clock::now();
    float dt  = std::chrono::duration<float>(now - last_time).count();
    last_time = now;

    server.update();
    ag::ScriptManager::call("on_tick", dt);

    auto elapsed = std::chrono::steady_clock::now() - now;
    if (elapsed < tick)
      std::this_thread::sleep_for(tick - elapsed);
  }

  ag::ScriptManager::call("on_server_stop");
  server.stop();

  return 0;
}
