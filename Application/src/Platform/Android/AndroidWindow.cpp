#ifdef PLATFORM_ANDROID

  #include <android/log.h>
  #include <android_native_app_glue.h>
  #include <EGL/egl.h>
  #include <GLES3/gl3.h>

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

AndroidWindow::AndroidWindow(const WindowProps& props, android_app* app)
  : m_app(app),
    m_title(props.Title)
{
  m_width  = props.Size.x;
  m_height = props.Size.y;

  m_app->userData = this;

  init_android_event_callbacks();

  AERO_CORE_INFO("AndroidWindow created: {} {}x{}", m_title.c_str(), m_width, m_height);
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
  AERO_CORE_INFO("Android event callbacks initialized");
}

bool AndroidWindow::init_egl()
{
  AERO_CORE_INFO("Initializing EGL...");

  m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (m_display == EGL_NO_DISPLAY)
  {
    AERO_CORE_ERROR("eglGetDisplay failed");
    return false;
  }

  if (eglInitialize(m_display, nullptr, nullptr) == EGL_FALSE)
  {
    AERO_CORE_ERROR("eglInitialize failed");
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

  EGLConfig config;
  EGLint    numConfigs;
  if (eglChooseConfig(m_display, attribs, &config, 1, &numConfigs) == EGL_FALSE || numConfigs == 0)
  {
    AERO_CORE_WARN("EGL ES 3.0 not available, trying ES 2.0...");

    // Fallback to ES 2.0
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

    if (eglChooseConfig(m_display, fallback_attribs, &config, 1, &numConfigs) == EGL_FALSE)
    {
      AERO_CORE_ERROR("eglChooseConfig failed for ES 2.0");
      return false;
    }
  }

  if (m_app->window == nullptr)
  {
    AERO_CORE_ERROR("Native window is null");
    return false;
  }

  m_surface = eglCreateWindowSurface(m_display, config, m_app->window, nullptr);
  if (m_surface == EGL_NO_SURFACE)
  {
    AERO_CORE_ERROR("eglCreateWindowSurface failed");
    return false;
  }

  const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
  m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, contextAttribs);

  if (m_context == EGL_NO_CONTEXT)
  {
    AERO_CORE_WARN("Failed to create ES 3.0 context, trying ES 2.0...");

    // Fallback to ES 2.0
    const EGLint es2_contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, es2_contextAttribs);

    if (m_context == EGL_NO_CONTEXT)
    {
      AERO_CORE_ERROR("eglCreateContext failed for both ES 3.0 and ES 2.0");
      return false;
    }
  }

  if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE)
  {
    AERO_CORE_ERROR("eglMakeCurrent failed");
    return false;
  }

  eglQuerySurface(m_display, m_surface, EGL_WIDTH, (EGLint*)&m_width);
  eglQuerySurface(m_display, m_surface, EGL_HEIGHT, (EGLint*)&m_height);

  glViewport(0, 0, m_width, m_height);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  set_vsync(m_vsync);

  m_initialized = true;
  AERO_CORE_INFO("EGL ready: {}x{}, vsync: {}", m_width, m_height, m_vsync ? "on" : "off");
  return true;
}

void AndroidWindow::destroy_egl()
{
  if (m_display != EGL_NO_DISPLAY)
  {
    eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    if (m_context != EGL_NO_CONTEXT)
    {
      eglDestroyContext(m_display, m_context);
      m_context = EGL_NO_CONTEXT;
    }
    if (m_surface != EGL_NO_SURFACE)
    {
      eglDestroySurface(m_display, m_surface);
      m_surface = EGL_NO_SURFACE;
    }
    eglTerminate(m_display);
    m_display = EGL_NO_DISPLAY;
  }
  m_initialized = false;
  AERO_CORE_INFO("EGL destroyed");
}

void AndroidWindow::on_update()
{
  eglSwapBuffers(m_display, m_surface);
}

void AndroidWindow::close_window()
{
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
  AERO_CORE_INFO("Window resized: {}x{}", m_width, m_height);
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
  {
    eglSwapInterval(m_display, enabled ? 1 : 0);
  }
}

void AndroidWindow::handle_android_cmd(int32_t cmd)
{
  switch (cmd)
  {
    case APP_CMD_INIT_WINDOW:
      AERO_CORE_INFO("APP_CMD_INIT_WINDOW");
      if (m_app->window != nullptr)
      {
        init_egl();
        // Trigger window show event

        if (m_callback)
        {
          WindowShowEvent event;
          m_callback(event);
        }
      }
      break;

    case APP_CMD_TERM_WINDOW:
      AERO_CORE_INFO("APP_CMD_TERM_WINDOW");

      destroy_egl();
      break;

    case APP_CMD_WINDOW_RESIZED:
      AERO_CORE_INFO("APP_CMD_WINDOW_RESIZED");
      if (is_egl_ready())
      {
        int width  = ANativeWindow_getWidth(m_app->window);
        int height = ANativeWindow_getHeight(m_app->window);
        set_size({(uint32_t)width, (uint32_t)height});
      }
      break;

    case APP_CMD_LOST_FOCUS:
      AERO_CORE_INFO("APP_CMD_LOST_FOCUS");
      break;

    case APP_CMD_GAINED_FOCUS:
      AERO_CORE_INFO("APP_CMD_GAINED_FOCUS");
      break;

    case APP_CMD_PAUSE:
      AERO_CORE_INFO("APP_CMD_PAUSE");
      break;

    case APP_CMD_RESUME:
      AERO_CORE_INFO("APP_CMD_RESUME");
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

int32_t AndroidWindow::handle_android_input(AInputEvent* event)
{
  if (!m_callback)
    return 0;

  int32_t event_type = AInputEvent_getType(event);

  switch (event_type)
  {
    case AINPUT_EVENT_TYPE_MOTION:
    {
      int32_t action        = AMotionEvent_getAction(event);
      int32_t pointer_index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                              AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
      int32_t pointer_id = AMotionEvent_getPointerId(event, pointer_index);
      float   x          = AMotionEvent_getX(event, pointer_index);
      float   y          = AMotionEvent_getY(event, pointer_index);

      // Convert to normalized coordinates (0-1 range)
      float norm_x = x / m_width;
      float norm_y = y / m_height;

      switch (action & AMOTION_EVENT_ACTION_MASK)
      {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        {
          m_touch_active = true;
          m_touch_id     = pointer_id;
          m_mouse_pos    = {norm_x, norm_y};

          MouseButtonPressedEvent btn_event(0);  // Left mouse button
          m_callback(btn_event);

          MouseMovedEvent move_event(norm_x, norm_y);
          m_callback(move_event);
          return 1;
        }

        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
        {
          if (pointer_id == m_touch_id)
          {
            m_touch_active = false;
            m_touch_id     = -1;

            MouseButtonReleasedEvent btn_event(0);
            m_callback(btn_event);
          }
          return 1;
        }

        case AMOTION_EVENT_ACTION_MOVE:
        {
          if (m_touch_active && pointer_id == m_touch_id)
          {
            m_mouse_pos = {norm_x, norm_y};
            MouseMovedEvent move_event(norm_x, norm_y);
            m_callback(move_event);
          }
          return 1;
        }
      }
      break;
    }

    case AINPUT_EVENT_TYPE_KEY:
    {
      int32_t key_code = AKeyEvent_getKeyCode(event);
      int32_t action   = AKeyEvent_getAction(event);

      // Map Android key codes to your key codes
      int mapped_key = key_code;

      switch (action)
      {
        case AKEY_EVENT_ACTION_DOWN:
        {
          KeyPressedEvent key_event(mapped_key, 0);
          m_callback(key_event);
          return 1;
        }
        case AKEY_EVENT_ACTION_UP:
        {
          KeyReleasedEvent key_event(mapped_key);
          m_callback(key_event);
          return 1;
        }
      }
      break;
    }
  }

  return 0;  // Event not handled
}

// Static callback handlers
static void static_handle_android_cmd(struct android_app* app, int32_t cmd)
{
  AndroidWindow* window = static_cast<AndroidWindow*>(app->userData);
  if (window)
  {
    window->handle_android_cmd(cmd);
  }
}

static int32_t static_handle_android_input(struct android_app* app, AInputEvent* event)
{
  AndroidWindow* window = static_cast<AndroidWindow*>(app->userData);
  if (window)
  {
    return window->handle_android_input(event);
  }
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

  if (g_android_app->window != nullptr)
  {
    AERO_CORE_INFO("Window available, initializing EGL in factory");
    window->init_egl();
  }

  return window;
}

}  // namespace ag

#endif  // PLATFORM_ANDROID
