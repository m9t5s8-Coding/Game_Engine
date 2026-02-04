#ifdef PLATFORM_ANDROID

#include <android_native_app_glue.h>
#include <Core/Log.hpp>
#include <Core/Application.hpp>
#include <Platform/Android/AndroidWindow.hpp>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

static bool g_Initialized = false;

static void handle_cmd(android_app* app, int32_t cmd)
{
  switch (cmd)
  {
  case APP_CMD_INIT_WINDOW:
  {
    if (app->window != nullptr && !g_Initialized)
    {
      AG_CORE_INFO("APP_CMD_INIT_WINDOW - Initializing application");

      // Create your application
      ag::Application* application = ag::CreateApplication();

      // Initialize the window
      auto* androidWindow = dynamic_cast<ag::AndroidWindow*>(&application->GetWindow());
      if (androidWindow)
      {
        androidWindow->initialize(app);
      }

      g_Initialized = true;

      // Store application in app->userData for later access
      app->userData = application;
    }
    break;
  }

  case APP_CMD_TERM_WINDOW:
  {
    AG_CORE_INFO("APP_CMD_TERM_WINDOW - Terminating");
    if (app->userData)
    {
      ag::Application* application = static_cast<ag::Application*>(app->userData);
      delete application;
      app->userData = nullptr;
    }
    g_Initialized = false;
    break;
  }

  case APP_CMD_LOST_FOCUS:
  {
    AG_CORE_INFO("APP_CMD_LOST_FOCUS");
    // Pause rendering/updates
    break;
  }

  case APP_CMD_GAINED_FOCUS:
  {
    AG_CORE_INFO("APP_CMD_GAINED_FOCUS");
    // Resume rendering/updates
    break;
  }
  }
}

static int32_t handle_input(android_app* app, AInputEvent* event)
{
  // Handle touch and key events here
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
  {
    // Touch input
    int32_t action = AMotionEvent_getAction(event);
    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);

    // Convert to your event system
    // ...

    return 1; // Event handled
  }

  return 0; // Event not handled
}

// Main entry point for Android
void android_main(android_app* app)
{
  AG_CORE_INFO("Android application starting...");

  // Set event handlers
  app->onAppCmd = handle_cmd;
  app->onInputEvent = handle_input;

  // Main loop
  while (true)
  {
    int events;
    android_poll_source* source;

    // Poll events with timeout
    while (ALooper_pollAll(g_Initialized ? 0 : -1, nullptr, &events, (void**)&source) >= 0)
    {
      if (source != nullptr)
      {
        source->process(app, source);
      }

      // Check if we need to exit
      if (app->destroyRequested != 0)
      {
        AG_CORE_INFO("Destroy requested, exiting");
        if (app->userData)
        {
          ag::Application* application = static_cast<ag::Application*>(app->userData);
          delete application;
          app->userData = nullptr;
        }
        return;
      }
    }

    // Run application update if initialized
    if (g_Initialized && app->userData)
    {
      ag::Application* application = static_cast<ag::Application*>(app->userData);

      // Clear screen
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Run your application loop
      application->Run(); // Or OnUpdate() depending on your architecture

      // Swap buffers (handled in AndroidWindow::on_update())
    }
  }
}

#endif // PLATFORM_ANDROID