#pragma once

#include <array>

namespace ag::script_hooks {

// Component script hooks. These names are part of the Lua-facing API and must
// stay stable even if the C++ call sites move between files.
inline constexpr const char* OnCreate          = "on_create";
inline constexpr const char* OnUpdate          = "on_update";
inline constexpr const char* OnDestroy         = "on_destroy";
inline constexpr const char* OnEvent           = "on_event";
inline constexpr const char* OnConnected       = "on_connected";
inline constexpr const char* OnDisconnected    = "on_disconnected";
inline constexpr const char* OnPacketReceived  = "on_packet_received";
// Draw hooks are bound separately so existing generated scripts and hook lists
// keep their original contents.
inline constexpr const char* OnDraw           = "on_draw";
// Mobile lifecycle hooks. Like OnDraw these are loaded explicitly so they do
// not change the desktop hook lists shown by the editor inspector.
inline constexpr const char* OnPause          = "on_pause";
inline constexpr const char* OnResume         = "on_resume";

inline constexpr std::array<const char*, 7> component = {
    OnCreate,
    OnUpdate,
    OnDestroy,
    OnEvent,
    OnConnected,
    OnDisconnected,
    OnPacketReceived,
};

// Global scripts only use the create/update pair.
inline constexpr std::array<const char*, 2> global = {
    OnCreate,
    OnUpdate,
};

// Server scripts receive these hooks through ScriptManager::call().
inline constexpr const char* OnClientConnected    = "on_client_connected";
inline constexpr const char* OnClientDisconnected = "on_client_disconnected";
inline constexpr const char* OnServerStart        = "on_server_start";
inline constexpr const char* OnTick               = "on_tick";
inline constexpr const char* OnServerStop         = "on_server_stop";

inline constexpr std::array<const char*, 6> server = {
    OnClientConnected,
    OnClientDisconnected,
    OnPacketReceived,
    OnServerStart,
    OnTick,
    OnServerStop,
};

}  // namespace ag::script_hooks
