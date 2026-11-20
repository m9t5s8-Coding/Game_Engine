#ifdef PLATFORM_ANDROID

  #include <android/keycodes.h>
  #include <android/log.h>
  #include <android_native_app_glue.h>
  #include <EGL/egl.h>
  #include <GLES3/gl3.h>

  #include <algorithm>
  #include <cmath>

  #include <Core/Application.hpp>
  #include <Core/Log.hpp>
  #include <Events/KeyEvent.hpp>
  #include <Events/MouseEvent.hpp>
  #include <Events/TextInput.hpp>
  #include <Events/WindowEvent.hpp>
  #include <Platform/Android/AndroidPlatform.hpp>
  #include <Platform/Android/AndroidWindow.hpp>

namespace ag
{

static void    static_handle_android_cmd(struct android_app* app, int32_t cmd);
static int32_t static_handle_android_input(struct android_app* app, AInputEvent* event);

namespace
{
constexpr size_t kMaxTouchPoints = 10;

KeyCode map_android_key(int32_t key_code)
{
  switch (key_code)
  {
    case AKEYCODE_BACK: return Key::Escape;
    case AKEYCODE_DPAD_UP: return Key::Up;
    case AKEYCODE_DPAD_DOWN: return Key::Down;
    case AKEYCODE_DPAD_LEFT: return Key::Left;
    case AKEYCODE_DPAD_RIGHT: return Key::Right;
    case AKEYCODE_DPAD_CENTER:
    case AKEYCODE_ENTER: return Key::Enter;
    case AKEYCODE_TAB: return Key::Tab;
    case AKEYCODE_SPACE: return Key::Space;
    case AKEYCODE_DEL: return Key::Backspace;
    case AKEYCODE_FORWARD_DEL: return Key::Delete;
    case AKEYCODE_INSERT: return Key::Insert;
    case AKEYCODE_MOVE_HOME: return Key::Home;
    case AKEYCODE_MOVE_END: return Key::End;
    case AKEYCODE_PAGE_UP: return Key::PageUp;
    case AKEYCODE_PAGE_DOWN: return Key::PageDown;
    case AKEYCODE_CAPS_LOCK: return Key::CapsLock;
    case AKEYCODE_SCROLL_LOCK: return Key::ScrollLock;
    case AKEYCODE_NUM_LOCK: return Key::NumLock;
    case AKEYCODE_ESCAPE: return Key::Escape;
    case AKEYCODE_SHIFT_LEFT: return Key::LeftShift;
    case AKEYCODE_SHIFT_RIGHT: return Key::RightShift;
    case AKEYCODE_CTRL_LEFT: return Key::LeftControl;
    case AKEYCODE_CTRL_RIGHT: return Key::RightControl;
    case AKEYCODE_ALT_LEFT: return Key::LeftAlt;
    case AKEYCODE_ALT_RIGHT: return Key::RightAlt;
    case AKEYCODE_COMMA: return Key::Comma;
    case AKEYCODE_PERIOD: return Key::Period;
    case AKEYCODE_MINUS: return Key::Minus;
    case AKEYCODE_PLUS: return Key::Equal;
    case AKEYCODE_SLASH: return Key::Slash;
    case AKEYCODE_SEMICOLON: return Key::Semicolon;
    case AKEYCODE_APOSTROPHE: return Key::Apostrophe;
    case AKEYCODE_EQUALS: return Key::Equal;
    case AKEYCODE_LEFT_BRACKET: return Key::LeftBracket;
    case AKEYCODE_RIGHT_BRACKET: return Key::RightBracket;
    case AKEYCODE_GRAVE: return Key::GraveAccent;
    case AKEYCODE_BACKSLASH: return Key::Backslash;
    case AKEYCODE_MENU: return Key::Menu;
    default: break;
  }

  if (key_code >= AKEYCODE_A && key_code <= AKEYCODE_Z)
    return static_cast<KeyCode>(Key::A + (key_code - AKEYCODE_A));
  if (key_code >= AKEYCODE_0 && key_code <= AKEYCODE_9)
    return static_cast<KeyCode>(Key::D0 + (key_code - AKEYCODE_0));
  if (key_code >= AKEYCODE_F1 && key_code <= AKEYCODE_F12)
    return static_cast<KeyCode>(Key::F1 + (key_code - AKEYCODE_F1));
  if (key_code >= AKEYCODE_NUMPAD_0 && key_code <= AKEYCODE_NUMPAD_9)
    return static_cast<KeyCode>(Key::KP0 + (key_code - AKEYCODE_NUMPAD_0));
  if (key_code == AKEYCODE_NUMPAD_DOT)
    return Key::KPDecimal;
  if (key_code == AKEYCODE_NUMPAD_DIVIDE)
    return Key::KPDivide;
  if (key_code == AKEYCODE_NUMPAD_MULTIPLY)
    return Key::KPMultiply;
  if (key_code == AKEYCODE_NUMPAD_SUBTRACT)
    return Key::KPSubtract;
  if (key_code == AKEYCODE_NUMPAD_ADD)
    return Key::KPAdd;
  if (key_code == AKEYCODE_NUMPAD_ENTER)
    return Key::KPEnter;
  if (key_code == AKEYCODE_NUMPAD_EQUALS)
    return Key::KPEqual;

  return 0;
}
}  // namespace

AndroidWindow::AndroidWindow(const WindowProps& props, android_app* app)
  : m_app(app),
    m_title(props.Title)
{
  m_width  = props.Size.x;
  m_height = props.Size.y;

  m_app->userData = this;

  init_android_event_callbacks();
}

AndroidWindow::~AndroidWindow()
{
  destroy_egl();
  AERO_CORE_INFO("AndroidWindow destroyed");
}

void AndroidWindow::init_android_event_callbacks()
{
  m_app->onAppCmd     = static_handle_android_cmd;
  m_app->onInputEvent = static_handle_android_input;
}

vec2u AndroidWindow::query_surface_size() const
{
  if (m_app->window)
  {
    const int width  = ANativeWindow_getWidth(m_app->window);
    const int height = ANativeWindow_getHeight(m_app->window);
    if (width > 0 && height > 0)
      return {static_cast<AG_uint>(width), static_cast<AG_uint>(height)};
  }

  if (m_surface != EGL_NO_SURFACE)
  {
    EGLint width  = 0;
    EGLint height = 0;
    eglQuerySurface(m_display, m_surface, EGL_WIDTH, &width);
    eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &height);
    if (width > 0 && height > 0)
      return {static_cast<AG_uint>(width), static_cast<AG_uint>(height)};
  }

  return {};
}

bool AndroidWindow::init_egl()
{
  if (m_surface_ready && m_native_window == m_app->window)
    return true;

  if (m_app->window == nullptr)
  {
    AERO_CORE_ERROR("Native window is null");
    return false;
  }

  if (m_surface != EGL_NO_SURFACE && m_native_window != m_app->window)
    release_surface();

  AERO_CORE_INFO("Initializing EGL...");

  const bool needs_setup = m_display == EGL_NO_DISPLAY;
  if (needs_setup)
  {
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_display == EGL_NO_DISPLAY)
    {
      AERO_CORE_ERROR("eglGetDisplay failed");
      return false;
    }

    if (eglInitialize(m_display, nullptr, nullptr) == EGL_FALSE)
    {
      AERO_CORE_ERROR("eglInitialize failed");
      destroy_egl();
      return false;
    }

    const EGLint attribs[] = {EGL_RENDERABLE_TYPE,
                              EGL_OPENGL_ES3_BIT,
                              EGL_SURFACE_TYPE,
                              EGL_WINDOW_BIT,
                              EGL_BLUE_SIZE,
                              8,
                              EGL_GREEN_SIZE,
                              8,
                              EGL_RED_SIZE,
                              8,
                              EGL_ALPHA_SIZE,
                              8,
                              EGL_DEPTH_SIZE,
                              24,
                              EGL_STENCIL_SIZE,
                              8,
                              EGL_NONE};

    EGLint numConfigs = 0;
    if (eglChooseConfig(m_display, attribs, &m_config, 1, &numConfigs) == EGL_FALSE || numConfigs == 0)
    {
      AERO_CORE_WARN("EGL ES 3.0 not available, trying ES 2.0...");

      const EGLint fallback_attribs[] = {EGL_RENDERABLE_TYPE,
                                         EGL_OPENGL_ES2_BIT,
                                         EGL_SURFACE_TYPE,
                                         EGL_WINDOW_BIT,
                                         EGL_BLUE_SIZE,
                                         8,
                                         EGL_GREEN_SIZE,
                                         8,
                                         EGL_RED_SIZE,
                                         8,
                                         EGL_ALPHA_SIZE,
                                         8,
                                         EGL_DEPTH_SIZE,
                                         24,
                                         EGL_STENCIL_SIZE,
                                         8,
                                         EGL_NONE};

      if (eglChooseConfig(m_display, fallback_attribs, &m_config, 1, &numConfigs) == EGL_FALSE)
      {
        AERO_CORE_ERROR("eglChooseConfig failed for ES 2.0");
        destroy_egl();
        return false;
      }
    }
  }

  m_surface = eglCreateWindowSurface(m_display, m_config, m_app->window, nullptr);
  if (m_surface == EGL_NO_SURFACE)
  {
    AERO_CORE_ERROR("eglCreateWindowSurface failed");
    if (needs_setup)
      destroy_egl();
    return false;
  }

  if (m_context == EGL_NO_CONTEXT)
  {
    const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, contextAttribs);

    if (m_context == EGL_NO_CONTEXT)
    {
      AERO_CORE_WARN("Failed to create ES 3.0 context, trying ES 2.0...");

      const EGLint es2_contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
      m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, es2_contextAttribs);
    }

    if (m_context == EGL_NO_CONTEXT)
    {
      AERO_CORE_ERROR("eglCreateContext failed for both ES 3.0 and ES 2.0");
      eglDestroySurface(m_display, m_surface);
      m_surface = EGL_NO_SURFACE;
      if (needs_setup)
        destroy_egl();
      return false;
    }
  }

  if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE)
  {
    AERO_CORE_ERROR("eglMakeCurrent failed");
    eglDestroySurface(m_display, m_surface);
    m_surface = EGL_NO_SURFACE;
    if (needs_setup)
      destroy_egl();
    return false;
  }

  const vec2u surface_size = query_surface_size();
  if (surface_size.x > 0 && surface_size.y > 0)
  {
    m_width  = surface_size.x;
    m_height = surface_size.y;
  }

  glViewport(0, 0, m_width, m_height);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  set_vsync(m_vsync);

  m_native_window  = m_app->window;
  m_initialized    = true;
  m_surface_ready  = true;
  return true;
}

void AndroidWindow::release_surface()
{
  if (m_display != EGL_NO_DISPLAY)
  {
    eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (m_surface != EGL_NO_SURFACE)
    {
      eglDestroySurface(m_display, m_surface);
      m_surface = EGL_NO_SURFACE;
    }
  }

  m_initialized   = false;
  m_surface_ready = false;
  m_native_window = nullptr;
}

void AndroidWindow::destroy_egl()
{
  const bool had_context = m_context != EGL_NO_CONTEXT;
  const bool had_display = m_display != EGL_NO_DISPLAY;

  release_surface();

  if (had_context)
  {
    eglDestroyContext(m_display, m_context);
    m_context = EGL_NO_CONTEXT;
  }
  if (had_display)
  {
    eglTerminate(m_display);
    m_display = EGL_NO_DISPLAY;
  }
  m_config = nullptr;

  if (had_display || had_context)
    AERO_CORE_INFO("EGL destroyed");
}

void AndroidWindow::render_dark_frame()
{
  if (!is_egl_ready())
    return;

  if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE)
    return;

  glViewport(0, 0, m_width, m_height);
  glDisable(GL_SCISSOR_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  eglSwapBuffers(m_display, m_surface);
}

void AndroidWindow::on_update()
{
  if (!is_egl_ready())
    return;
  eglSwapBuffers(m_display, m_surface);
}

void AndroidWindow::close_window()
{
  render_dark_frame();
  destroy_egl();
}

void AndroidWindow::set_size(const vec2u& size)
{
  m_width  = size.x;
  m_height = size.y;
  if (m_initialized)
  {
    glViewport(0, 0, m_width, m_height);
    if (m_callback)
    {
      WindowResizeEvent event(size);
      m_callback(event);
    }
  }
}

void AndroidWindow::set_position(const vec2i& position)
{
  (void)position;
}

vec2u AndroidWindow::get_position() const
{
  return {0, 0};
}

void AndroidWindow::show_decoration(bool show)
{
  (void)show;
}

void AndroidWindow::center_window()
{
}

void AndroidWindow::set_full_screen()
{
}

void AndroidWindow::set_vsync(bool enabled)
{
  m_vsync = enabled;
  if (m_display != EGL_NO_DISPLAY)
    eglSwapInterval(m_display, enabled ? 1 : 0);
}

void AndroidWindow::clear_input_state()
{
  m_touch_points.clear();
  m_touch_active        = false;
  m_touch_id            = -1;
  m_pinching            = false;
  m_last_pinch_distance = 0.0f;
  m_key_state.fill(false);
}

void AndroidWindow::update_app_activity()
{
  const bool active = m_app_resumed && m_surface_ready;
  if (active == m_app_active)
    return;

  m_app_active = active;

  Application& app = Application::get();
  app.set_app_active(active);
  if (active)
    app.on_app_resume();
  else
    app.on_app_pause();
}

void AndroidWindow::handle_android_cmd(int32_t cmd)
{
  switch (cmd)
  {
    case APP_CMD_INIT_WINDOW:
      AERO_CORE_INFO("APP_CMD_INIT_WINDOW");
      if (m_app->window != nullptr && init_egl())
      {
        // Present black before the first game frame so a resume or app
        // switcher snapshot never shows the previous scene.
        render_dark_frame();

        const vec2u size = query_surface_size();
        if (size.x > 0 && size.y > 0)
          set_size(size);

        if (m_callback)
        {
          WindowShowEvent show_event;
          m_callback(show_event);
        }
        update_app_activity();
      }
      break;

    case APP_CMD_TERM_WINDOW:
      AERO_CORE_INFO("APP_CMD_TERM_WINDOW");
      render_dark_frame();
      m_surface_ready = false;
      update_app_activity();
      // Keep the EGL context so renderer resources survive the app switcher.
      release_surface();
      break;

    case APP_CMD_WINDOW_RESIZED:
    case APP_CMD_CONFIG_CHANGED:
      if (is_egl_ready())
      {
        const vec2u size = query_surface_size();
        if (size.x > 0 && size.y > 0)
          set_size(size);
        render_dark_frame();
      }
      break;

    case APP_CMD_LOST_FOCUS:
      AERO_CORE_INFO("APP_CMD_LOST_FOCUS");
      render_dark_frame();
      m_app_resumed = false;
      update_app_activity();
      break;

    case APP_CMD_GAINED_FOCUS:
      AERO_CORE_INFO("APP_CMD_GAINED_FOCUS");
      m_app_resumed = true;
      update_app_activity();
      render_dark_frame();
      break;

    case APP_CMD_PAUSE:
      AERO_CORE_INFO("APP_CMD_PAUSE");
      render_dark_frame();
      clear_input_state();
      m_app_resumed = false;
      update_app_activity();
      break;

    case APP_CMD_RESUME:
      AERO_CORE_INFO("APP_CMD_RESUME");
      m_app_resumed = true;
      update_app_activity();
      render_dark_frame();
      break;

    case APP_CMD_START:
      AERO_CORE_INFO("APP_CMD_START");
      break;

    case APP_CMD_STOP:
      AERO_CORE_INFO("APP_CMD_STOP");
      render_dark_frame();
      clear_input_state();
      m_app_resumed = false;
      update_app_activity();
      break;

    case APP_CMD_DESTROY:
      AERO_CORE_INFO("APP_CMD_DESTROY");
      close_window();
      break;

    default:
      AERO_CORE_INFO("Unhandled cmd: {}", cmd);
      break;
  }
}

void AndroidWindow::sync_touch_points(AInputEvent* event)
{
  m_touch_points.clear();

  const size_t count = std::min<size_t>(static_cast<size_t>(AMotionEvent_getPointerCount(event)),
                                        kMaxTouchPoints);
  for (size_t i = 0; i < count; ++i)
  {
    m_touch_points.push_back({AMotionEvent_getPointerId(event, i),
                              {AMotionEvent_getX(event, i), AMotionEvent_getY(event, i)}});
  }
}

void AndroidWindow::remove_touch_point(int id)
{
  m_touch_points.erase(std::remove_if(m_touch_points.begin(),
                                      m_touch_points.end(),
                                      [id](const TouchPoint& point) { return point.id == id; }),
                       m_touch_points.end());

  if (m_touch_points.empty())
  {
    m_touch_active = false;
    m_touch_id     = -1;
    m_pinching     = false;
  }
  else
  {
    m_touch_id   = m_touch_points.front().id;
    m_mouse_pos  = m_touch_points.front().position;
    m_touch_active = true;
    m_pinching   = m_touch_points.size() >= 2;
  }
  m_last_pinch_distance = get_pinch_distance();
}

float AndroidWindow::get_pinch_distance() const
{
  if (m_touch_points.size() < 2)
    return 0.0f;

  const vec2f delta = m_touch_points[1].position - m_touch_points[0].position;
  return std::sqrt(delta.x * delta.x + delta.y * delta.y);
}

const TouchPoint* AndroidWindow::get_touch_point(size_t index) const
{
  if (index >= m_touch_points.size())
    return nullptr;
  return &m_touch_points[index];
}

bool AndroidWindow::is_key_down(KeyCode key) const
{
  if (key >= m_key_state.size())
    return false;
  return m_key_state[key];
}

int32_t AndroidWindow::handle_android_input(AInputEvent* event)
{
  if (!m_callback || !m_app_active)
    return 0;

  const int32_t event_type = AInputEvent_getType(event);

  switch (event_type)
  {
    case AINPUT_EVENT_TYPE_MOTION:
    {
      const int32_t action        = AMotionEvent_getAction(event);
      const int32_t pointer_index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                                    AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
      const int32_t pointer_id = AMotionEvent_getPointerId(event, pointer_index);
      const float   x          = AMotionEvent_getX(event, pointer_index);
      const float   y          = AMotionEvent_getY(event, pointer_index);

      switch (action & AMOTION_EVENT_ACTION_MASK)
      {
        case AMOTION_EVENT_ACTION_DOWN:
        {
          sync_touch_points(event);
          m_touch_id         = pointer_id;
          m_mouse_pos        = {x, y};
          m_touch_active     = true;
          m_pinching         = m_touch_points.size() >= 2;
          m_last_pinch_distance = get_pinch_distance();

          MouseButtonPressedEvent btn_event(Button::ButtonLeft);
          m_callback(btn_event);

          MouseMovedEvent move_event(x, y);
          m_callback(move_event);
          return 1;
        }

        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        {
          sync_touch_points(event);
          if (m_touch_points.size() >= 2)
          {
            m_pinching            = true;
            m_last_pinch_distance = get_pinch_distance();
          }
          return 1;
        }

        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
        {
          const int32_t lifted_id = pointer_id;
          sync_touch_points(event);
          remove_touch_point(lifted_id);

          MouseButtonReleasedEvent btn_event(Button::ButtonLeft);
          m_callback(btn_event);
          return 1;
        }

        case AMOTION_EVENT_ACTION_CANCEL:
        {
          clear_input_state();
          MouseButtonReleasedEvent btn_event(Button::ButtonLeft);
          m_callback(btn_event);
          return 1;
        }

        case AMOTION_EVENT_ACTION_MOVE:
        {
          sync_touch_points(event);

          for (const TouchPoint& point : m_touch_points)
          {
            if (point.id == m_touch_id)
            {
              m_mouse_pos = point.position;
              break;
            }
          }

          if (m_pinching && m_touch_points.size() >= 2)
          {
            const float current_distance = get_pinch_distance();
            if (m_last_pinch_distance > 0.0f && current_distance > 0.0f)
            {
              const float scale  = m_last_pinch_distance / current_distance;
              const float offset = (scale < 1.0f) ? 1.0f : -1.0f;  // zoom in or out
              MouseScrolledEvent scroll_event(0.0f, offset);
              m_callback(scroll_event);
            }
            m_last_pinch_distance = current_distance;
            return 1;
          }

          if (m_touch_active)
          {
            MouseMovedEvent move_event(m_mouse_pos.x, m_mouse_pos.y);
            m_callback(move_event);
          }
          return 1;
        }
      }
      break;
    }

    case AINPUT_EVENT_TYPE_KEY:
    {
      const int32_t key_code = AKeyEvent_getKeyCode(event);
      const int32_t action   = AKeyEvent_getAction(event);
      const KeyCode mapped_key = map_android_key(key_code);

      if (mapped_key == 0)
        return 0;

      if (action == AKEY_EVENT_ACTION_DOWN)
      {
        m_key_state[mapped_key] = true;
        KeyPressedEvent key_event(mapped_key, AKeyEvent_getRepeatCount(event));
        m_callback(key_event);
        return 1;
      }
      if (action == AKEY_EVENT_ACTION_UP)
      {
        m_key_state[mapped_key] = false;
        KeyReleasedEvent key_event(mapped_key);
        m_callback(key_event);
        return 1;
      }
      break;
    }
  }

  return 0;
}

// Static callback handlers
static void static_handle_android_cmd(struct android_app* app, int32_t cmd)
{
  AndroidWindow* window = static_cast<AndroidWindow*>(app->userData);
  if (window)
    window->handle_android_cmd(cmd);
}

static int32_t static_handle_android_input(struct android_app* app, AInputEvent* event)
{
  AndroidWindow* window = static_cast<AndroidWindow*>(app->userData);
  if (window)
    return window->handle_android_input(event);
  return 0;
}

// Factory method
Window* Window::create(const WindowProps& props)
{
  if (!g_android_app)
  {
    AERO_CORE_ERROR("g_android_app is null, cannot create window!");
    return nullptr;
  }

  AndroidWindow* window = new AndroidWindow(props, g_android_app);

  if (g_android_app->window != nullptr && window->init_egl())
    window->render_dark_frame();

  return window;
}

}  // namespace ag

#endif  // PLATFORM_ANDROID
