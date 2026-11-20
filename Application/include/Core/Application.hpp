#pragma once

#include <Apch.hpp>
#include <Audio/AudioDevice.hpp>
#include <Core/Input.hpp>
#include <Core/TimeStamp.hpp>
#include <Core/Windows.hpp>
#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>
#ifdef AERO_EDITOR
  #include <ImGui/ImGuiLayer.hpp>
#endif
#include <Layers/LayerStack.hpp>
#include <Renderer/View.hpp>
#include <Scripting/ScriptBinding/SignalManager.hpp>
#include <string>
#include <vector>

#ifdef PLATFORM_ANDROID
  #include <android_native_app_glue.h>
#endif

namespace ag {
class Application {
public:
  Application();
  virtual ~Application();

  // Command line values are captured by EntryPoint before create_application().
  static void set_command_line(int argc, char** argv);
  static std::string get_project_path_argument();

  void init(const WindowProps& props);
  // main loop
  void run();
  void run_frame();

  virtual void on_create()  = 0;
  virtual void on_destroy() = 0;

  void on_event(Event& e);
  void push_layer(Layer* layer);
  void pop_layer(Layer* layer);
  void push_overlay(Layer* overlay);

  inline static Application& get() { return *s_Instance; }
  inline Window&             get_window() { return *m_Window; }

  inline static void  set_mouse_position(const vec2f& mouse_pos) { s_mouse_position = mouse_pos; }
  inline static vec2f get_mouse_position() { return s_mouse_position; }
  static std::string  get_exe_directory();

#ifdef AERO_EDITOR
  ImGuiLayer* get_imgui_layer() { return m_imgui_layer; }
#endif

  bool is_minimized() const { return m_minimized; }
  // Android pauses the engine when the surface is gone or the app is in the
  // background. Desktop builds stay active unless a minimize event arrives.
  bool is_app_active() const { return m_app_active && !m_minimized; }

  void set_app_active(bool active) { m_app_active = active; }
  void on_app_pause();
  void on_app_resume();

  // bool is_window_close() const { return  }

  bool  m_running    = true;
  bool  m_is_closing = false;
  float delta_time   = 0.0f;

  void minimize_application(bool minimized) { m_minimized = minimized; }

protected:
  AG_scope<Window>       m_Window;
  AG_scope<Audio_Device> m_audio_device;

private:
  bool on_window_close(WindowCloseEvent& e);
  bool on_window_resize(WindowResizeEvent& e);
  bool on_key_pressed(KeyPressedEvent& e);

  bool m_minimized  = false;
  bool m_app_active = true;

#ifdef AERO_EDITOR
  ImGuiLayer* m_imgui_layer = nullptr;
#endif
  LayerStack m_layerstack;

  float m_last_frametime = 0.0f;

  Signal_Manager m_manager;

private:
  static Application*         s_Instance;
  static std::vector<std::string> s_command_line;
  inline static vec2f s_mouse_position;
};

Application* create_application();

#ifdef PLATFORM_ANDROID
Application* create_application(android_app* app);
#endif
}  // namespace ag
