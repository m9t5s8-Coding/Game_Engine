file(
  GLOB_RECURSE
  APP_CORE_SRC
  "${CMAKE_SOURCE_DIR}/Application/src/Audio/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/Core/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/Font/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/GameObjects/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/ImGui/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/Layers/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/Networking/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/Project/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/Renderer/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/Scene/*.cpp"
  "${CMAKE_SOURCE_DIR}/Application/src/Scripting/*.cpp")

if(EMSCRIPTEN)
  # Web builds reuse the GLFW window/input/time layer and the OpenGL ES
  # renderer/audio implementations, plus the POSIX-ish TCP socket code.
  file(
    GLOB
    APP_PLATFORM_SRC
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/WindowsWindow/WindowInput.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/WindowsWindow/WindowsTime.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/WindowsWindow/WindowsWindow.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenGL/OpenGLContext.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenGL_ES/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenAL/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/Linux/TCPSocket.cpp")
elseif(ANDROID)
  file(
    GLOB
    APP_PLATFORM_SRC
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/Android/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenAL/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenGL_ES/*.cpp")
  list(FILTER APP_PLATFORM_SRC EXCLUDE REGEX "android_main\\.cpp$")
elseif(UNIX)
  file(
    GLOB
    APP_PLATFORM_SRC
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/WindowsWindow/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenGL/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenAL/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/Linux/*.cpp")
elseif(WIN32)
  file(
    GLOB
    APP_PLATFORM_SRC
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/WindowsWindow/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenGL/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenAL/*.cpp")
endif()

set(APP_SRC ${APP_CORE_SRC} ${APP_PLATFORM_SRC})
if(NOT APP_PLATFORM_SRC)
  message(
    WARNING
      "No platform-specific sources found — falling back to glob all sources")
  file(GLOB_RECURSE APP_SRC "${CMAKE_SOURCE_DIR}/Application/src/*.cpp")
endif()

macro(apply_application_settings target_name)
  set_msvc_runtime_library(${target_name})

  target_include_directories(${target_name} BEFORE
                             PUBLIC ${CMAKE_SOURCE_DIR}/External/lua)

  target_include_directories(
    ${target_name}
    PUBLIC ${CMAKE_SOURCE_DIR}/Application/include
           ${CMAKE_SOURCE_DIR}/External/box2d/include
           ${CMAKE_SOURCE_DIR}/External/imgui
           ${CMAKE_SOURCE_DIR}/External/imgui/backends)

  if(ANDROID)
    target_include_directories(
      ${target_name} PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
  endif()

  target_link_libraries(${target_name} PUBLIC lua box2d imgui External OpenAL)

  target_compile_definitions(
    ${target_name}
    PRIVATE STB_IMAGE_IMPLEMENTATION DR_MP3_IMPLEMENTATION
            DR_WAV_IMPLEMENTATION $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>)

  if(UNIX
     AND NOT APPLE
     AND NOT ANDROID
     AND NOT EMSCRIPTEN)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_include_directories(${target_name} PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(${target_name} PRIVATE ${GTK3_LIBRARIES})
    target_compile_options(${target_name} PRIVATE ${GTK3_CFLAGS_OTHER})
  endif()

  if(ANDROID)
    target_link_libraries(
      ${target_name} PUBLIC ${log-lib} ${android-lib} ${EGL-lib} ${GLESv3-lib}
                            ${OpenSLES-lib})
    target_compile_definitions(${target_name} PUBLIC IMGUI_IMPL_OPENGL_ES3)
  else()
    if(WIN32)
      target_link_libraries(
        ${target_name}
        PRIVATE user32.lib
                gdi32.lib
                shell32.lib
                kernel32.lib
                winspool.lib
                ole32.lib
                oleaut32.lib
                uuid.lib
                comdlg32.lib
                advapi32.lib
                dxguid.lib
                winmm.lib
                dsound.lib
                oleacc.lib
                setupapi.lib)
    elseif(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
      target_link_libraries(${target_name} PRIVATE pthread m)
    endif()
  endif()

  target_precompile_headers(${target_name} PUBLIC
                            "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp")
endmacro()

if(AERO_DESKTOP AND BUILD_EDITOR)
  add_library(ApplicationEditor STATIC ${APP_SRC})
  apply_application_settings(ApplicationEditor)
  target_compile_definitions(ApplicationEditor PRIVATE AERO_EDITOR)
  message(STATUS "ApplicationEditor: AERO_EDITOR defined")
endif()

if(ANDROID OR EMSCRIPTEN OR BUILD_SANDBOX)
  add_library(ApplicationRuntime STATIC ${APP_SRC})
  apply_application_settings(ApplicationRuntime)
  target_compile_definitions(ApplicationRuntime PRIVATE AERO_RUNTIME)
  message(STATUS "ApplicationRuntime: AERO_RUNTIME defined")
endif()

if(AERO_DESKTOP AND BUILD_SERVER)
  add_library(ApplicationServer STATIC ${APP_SRC})
  apply_application_settings(ApplicationServer)
  target_compile_definitions(ApplicationServer PRIVATE AERO_SERVER)

  if(UNIX AND NOT APPLE)
    target_link_libraries(ApplicationServer PRIVATE pthread m)
  endif()

  if(WIN32)
    target_link_libraries(
      ApplicationServer
      PRIVATE user32.lib
              gdi32.lib
              shell32.lib
              kernel32.lib
              winspool.lib
              ole32.lib
              oleaut32.lib
              uuid.lib
              comdlg32.lib
              advapi32.lib
              dxguid.lib
              winmm.lib
              dsound.lib
              oleacc.lib
              setupapi.lib)
  endif()
endif()

if(AERO_DESKTOP AND UNIX AND NOT APPLE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
  message(STATUS "GTK3 found: ${GTK3_VERSION}")
endif()
