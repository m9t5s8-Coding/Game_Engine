#pragma once

#include <Aero.hpp>
#include <iostream>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Platform/OpenGL/OpenGLShader.hpp>

class Sandbox2D : public ag::Layer
{
public:
  Sandbox2D();
  virtual ~Sandbox2D() = default;

  virtual void on_attach() override;
  virtual void on_detach() override;

  virtual void on_update(ag::TimeStamp ts) override;
  virtual void on_imgui_render() override;
  virtual void on_event(ag::Event &e) override;

private:
  ag::AG_ref<ag::ViewController> m_view_controller;
  ag::AG_ref<ag::Texture> m_texture;

  ag::vec3f m_triangle_pos;
  ag::Color m_color;
};
