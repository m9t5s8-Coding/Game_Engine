#pragma once

#include <Apch.hpp>
#include <Audio/AudioDevice.hpp>
#include <Core/Input.hpp>
#include <Core/TimeStamp.hpp>
#include <Core/Windows.hpp>
#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>
#include <ImGui/ImGuiLayer.hpp>
#include <Layers/LayerStack.hpp>
#include <Renderer/IndexBuffer.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/View.hpp>
#include <Scripting/ScriptBinding/SignalManager.hpp>

namespace ag
{
class Application
{
   public:
    Application();
    virtual ~Application();

    void init(const WindowProps& props);
    // main loop
    void run();

    virtual void on_create() = 0;
    virtual void on_destroy() = 0;

    void on_event(Event& e);
    void push_layer(Layer* layer);
    void pop_layer(Layer* layer);
    void push_overlay(Layer* overlay);

    inline static Application& get() { return *s_Instance; }
    inline Window& get_window() { return *m_Window; }

    inline static void set_mouse_position(const vec2f& mouse_pos) { s_mouse_position = mouse_pos; }
    inline static vec2f get_mouse_position() { return s_mouse_position; }
    static std::string get_exe_directory();

    ImGuiLayer* get_imgui_layer() { return m_imgui_layer; }

    bool is_minimized() const { return m_minimized; }

    // bool is_window_close() const { return  }

    bool m_running = true;
    bool m_is_closing = false;
    float delta_time = 0.0f;

   protected:
    AG_scope<Window> m_Window;
    AG_scope<Audio_Device> m_audio_device;

   private:
    bool on_window_close(WindowCloseEvent& e);
    bool on_window_resize(WindowResizeEvent& e);
    bool on_key_pressed(KeyPressedEvent& e);

    bool m_minimized = false;

    ImGuiLayer* m_imgui_layer = nullptr;
    LayerStack m_layerstack;

    float m_last_frametime = 0.0f;

    Signal_Manager m_manager;

   private:
    static Application* s_Instance;
    inline static vec2f s_mouse_position;
};

Application* create_application();
}  // namespace ag
