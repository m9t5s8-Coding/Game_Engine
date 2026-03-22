#ifdef PLATFORM_ANDROID

  #include <android/log.h>
  #include <android_native_app_glue.h>

  #include <Core/Application.hpp>
  #include <Core/Log.hpp>
  #include <Platform/Android/AndroidPlatform.hpp>

namespace ag
{
android_app*   g_android_app   = nullptr;
AAssetManager* g_asset_manager = nullptr;

void init_android_platform(android_app* app)
{
  g_android_app   = app;
  g_asset_manager = app->activity->assetManager;
}
}  // namespace ag

void android_main(struct android_app* app)
{
  ag::init_android_platform(app);

  ag::Log::init_android();

  if (!app)
  {
    return;
  }

  int loop_count = 0;
  while (app->window == nullptr)
  {
    loop_count++;

    int                         events;
    struct android_poll_source* source;
    ALooper_pollAll(-1, nullptr, &events, (void**)&source);
    if (source)
      source->process(app, source);

    usleep(10000);
  }

  ag::Application* engine = ag::create_application(app);

  if (!engine)
    return;

  engine->init(ag::WindowProps("AeroEngine", {1280, 720}));

  engine->on_create();

  while (true)
  {
    int                         events;
    struct android_poll_source* source;

    while (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0)
    {
      if (source)
        source->process(app, source);
      if (app->destroyRequested)
      {
        goto shutdown;
      }
    }

    engine->run_frame();
  }

shutdown:
  engine->on_destroy();
  delete engine;
}
#endif  // PLATFORM_ANDROID
