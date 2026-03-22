#pragma once

#ifdef PLATFORM_ANDROID
  #include <android/asset_manager.h>
  #include <android_native_app_glue.h>
namespace ag
{
extern android_app*   g_android_app;
extern AAssetManager* g_asset_manager;
void                  init_android_platform(android_app* app);
}  // namespace ag
#endif
