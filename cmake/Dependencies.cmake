if(NOT ANDROID AND NOT EMSCRIPTEN)
  message(STATUS "Configuring desktop libraries...")

  # --- GLFW (static) ---
  set(GLFW_BUILD_DOCS
      OFF
      CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS
      OFF
      CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES
      OFF
      CACHE BOOL "" FORCE)
  set(GLFW_INSTALL
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_SHARED_LIBS
      OFF
      CACHE BOOL "" FORCE)
  add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw)
  set_msvc_runtime_library(glfw)

endif()

# --- OpenAL-Soft (all platforms including Android) ---
if(EMSCRIPTEN)
  # Emscripten ships an OpenAL implementation; link it with -lopenal.
  add_library(OpenAL INTERFACE)
  target_link_libraries(OpenAL INTERFACE openal)
else()
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(ALSOFT_UTILS
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_NO_CONFIG_UTIL
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_CONFIG
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS
    OFF
    CACHE BOOL "" FORCE)
set(LIBTYPE
    "STATIC"
    CACHE STRING "Library type" FORCE)
set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "" FORCE)

if(ANDROID)
  # Android system libraries, shared by the Application libraries and the
  # native runtime entry point.
  find_library(log-lib log)
  find_library(android-lib android)
  find_library(EGL-lib EGL)
  find_library(GLESv3-lib GLESv3)
  find_library(OpenSLES-lib OpenSLES)

  # Android: use OpenSL ES backend
  set(ALSOFT_REQUIRE_OPENSL
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_OPENSL
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_WINMM
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_DSOUND
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_WASAPI
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_ALSA
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_PULSEAUDIO
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_COREAUDIO
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_REQUIRE_OPENSL
      ON
      CACHE BOOL "" FORCE)
elseif(WIN32)
  set(ALSOFT_REQUIRE_WINMM
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_REQUIRE_DSOUND
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_REQUIRE_WASAPI
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_WINMM
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_DSOUND
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_WASAPI
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_ALSA
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_PULSEAUDIO
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_COREAUDIO
      OFF
      CACHE BOOL "" FORCE)
elseif(UNIX AND NOT APPLE)
  set(ALSOFT_BACKEND_ALSA
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_PULSEAUDIO
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_WINMM
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_DSOUND
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_WASAPI
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_COREAUDIO
      OFF
      CACHE BOOL "" FORCE)
elseif(APPLE)
  set(ALSOFT_BACKEND_COREAUDIO
      ON
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_WINMM
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_DSOUND
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_WASAPI
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_ALSA
      OFF
      CACHE BOOL "" FORCE)
  set(ALSOFT_BACKEND_PULSEAUDIO
      OFF
      CACHE BOOL "" FORCE)
endif()

set(ALSOFT_BACKEND_PORTAUDIO
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_OSS
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_SNDIO
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_SOLARIS
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_SOUNDIO
    OFF
    CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_WAVE
    OFF
    CACHE BOOL "" FORCE)

if(MSVC)
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY
      "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>\
$<$<CONFIG:Release>:MultiThreadedDLL>\
$<$<CONFIG:RelWithDebInfo>:MultiThreadedDLL>\
$<$<CONFIG:MinSizeRel>:MultiThreadedDLL>\
$<$<CONFIG:Distributed>:MultiThreadedDLL>")  # Changed to /MD
endif()

if(MSVC)
  add_subdirectory(${CMAKE_SOURCE_DIR}/External/openal)

  get_directory_property(
    openal_targets DIRECTORY ${CMAKE_SOURCE_DIR}/External/openal
                             BUILDSYSTEM_TARGETS)
  foreach(tgt ${openal_targets})
    if(TARGET ${tgt})
      get_target_property(tgt_type ${tgt} TYPE)
      if(tgt_type MATCHES
         "STATIC_LIBRARY|SHARED_LIBRARY|OBJECT_LIBRARY|MODULE_LIBRARY")
        set_target_properties(
          ${tgt}
          PROPERTIES
            MSVC_RUNTIME_LIBRARY
            "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>\
$<$<CONFIG:Release>:MultiThreadedDLL>\
$<$<CONFIG:RelWithDebInfo>:MultiThreadedDLL>\
$<$<CONFIG:MinSizeRel>:MultiThreadedDLL>\
$<$<CONFIG:Distributed>:MultiThreadedDLL>")  # Changed to /MD
      endif()
    endif()
  endforeach()

  foreach(internal_tgt alsoft-common common alcommon fmt)
    if(TARGET ${internal_tgt})
      get_target_property(aliased ${internal_tgt} ALIASED_TARGET)
      if(NOT aliased)
        set_target_properties(
          ${internal_tgt}
          PROPERTIES
            MSVC_RUNTIME_LIBRARY
            "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>\
$<$<CONFIG:Release>:MultiThreadedDLL>\
$<$<CONFIG:RelWithDebInfo>:MultiThreadedDLL>\
$<$<CONFIG:MinSizeRel>:MultiThreadedDLL>\
$<$<CONFIG:Distributed>:MultiThreadedDLL>")  # Changed to /MD
      endif()
    endif()
  endforeach()

  if(WIN32)
    target_link_libraries(OpenAL INTERFACE legacy_stdio_definitions.lib)
  endif()
else()
  add_subdirectory(${CMAKE_SOURCE_DIR}/External/openal)
endif()
endif() # EMSCRIPTEN

# Print OpenAL diagnostic
get_target_property(_openal_type OpenAL TYPE)
get_target_property(_openal_includes OpenAL INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "OpenAL TYPE     = ${_openal_type}")
message(STATUS "OpenAL INCLUDES = ${_openal_includes}")

# ── Lua ───────────────────────────────────────────────────────────────────────
set(LUA_SRC
    "${CMAKE_SOURCE_DIR}/External/lua/lapi.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lauxlib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lbaselib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lcode.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lcorolib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lctype.c"
    "${CMAKE_SOURCE_DIR}/External/lua/ldblib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/ldebug.c"
    "${CMAKE_SOURCE_DIR}/External/lua/ldo.c"
    "${CMAKE_SOURCE_DIR}/External/lua/ldump.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lfunc.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lgc.c"
    "${CMAKE_SOURCE_DIR}/External/lua/linit.c"
    "${CMAKE_SOURCE_DIR}/External/lua/liolib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/llex.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lmathlib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lmem.c"
    "${CMAKE_SOURCE_DIR}/External/lua/loadlib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lobject.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lopcodes.c"
    "${CMAKE_SOURCE_DIR}/External/lua/loslib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lparser.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lstate.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lstring.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lstrlib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/ltable.c"
    "${CMAKE_SOURCE_DIR}/External/lua/ltablib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/ltm.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lundump.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lutf8lib.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lvm.c"
    "${CMAKE_SOURCE_DIR}/External/lua/lzio.c")

add_library(lua STATIC ${LUA_SRC})
set_target_properties(lua PROPERTIES LINKER_LANGUAGE C)
target_include_directories(lua PUBLIC "${CMAKE_SOURCE_DIR}/External/lua")

if(EMSCRIPTEN)
  target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3)
elseif(ANDROID)
  target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3 LUA_USE_POSIX)
elseif(UNIX)
  target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3 LUA_USE_POSIX
                                         LUA_USE_DLOPEN)
  if(NOT APPLE)
    target_link_libraries(lua PUBLIC dl)
  endif()
else()
  target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3 _CRT_SECURE_NO_WARNINGS)
endif()

set_msvc_runtime_library(lua)

file(GLOB_RECURSE BOX2D_SRC "${CMAKE_SOURCE_DIR}/External/box2d/src/*.cpp")
add_library(box2d STATIC ${BOX2D_SRC})
set_target_properties(box2d PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(box2d
                           PUBLIC "${CMAKE_SOURCE_DIR}/External/box2d/include")
set_msvc_runtime_library(box2d)

set(IMGUI_CORE_SRC
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_demo.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_tables.cpp")

if(ANDROID)
  set(IMGUI_BACKEND_SRC
      "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_android.cpp"
      "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp")
else()
  set(IMGUI_BACKEND_SRC
      "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_glfw.cpp"
      "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp")
endif()

add_library(imgui STATIC ${IMGUI_CORE_SRC} ${IMGUI_BACKEND_SRC})
target_include_directories(
  imgui PUBLIC ${CMAKE_SOURCE_DIR}/External/imgui
               ${CMAKE_SOURCE_DIR}/External/imgui/backends)
if(NOT ANDROID AND NOT EMSCRIPTEN)
  target_include_directories(imgui
                             PUBLIC ${CMAKE_SOURCE_DIR}/External/glfw/include)
endif()

if(ANDROID)
  target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_ES3)
elseif(EMSCRIPTEN)
  target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_ES3)
else()
  target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
endif()

set_msvc_runtime_library(imgui)

if(ANDROID OR EMSCRIPTEN)
  add_library(External INTERFACE)
  target_include_directories(
    External
    INTERFACE ${CMAKE_SOURCE_DIR}/External/entt/include
              ${CMAKE_SOURCE_DIR}/External/glm
              ${CMAKE_SOURCE_DIR}/External/spdlog/include
              ${CMAKE_SOURCE_DIR}/External/imgui
              ${CMAKE_SOURCE_DIR}/External/imgui/backends
              ${CMAKE_SOURCE_DIR}/External/stb_image
              ${CMAKE_SOURCE_DIR}/External/json/single_include
              ${CMAKE_SOURCE_DIR}/External/sol2/include
              ${CMAKE_SOURCE_DIR}/External/openal/include
              ${CMAKE_SOURCE_DIR}/External/audio
              ${CMAKE_SOURCE_DIR}/External/icons)

else()
  set(GLAD_SRC "${CMAKE_SOURCE_DIR}/External/glad/glad.cpp")
  add_library(External STATIC ${GLAD_SRC})
  set_msvc_runtime_library(External)

  target_include_directories(
    External
    PUBLIC ${CMAKE_SOURCE_DIR}/External/entt/include
           ${CMAKE_SOURCE_DIR}/External/glfw/include
           ${CMAKE_SOURCE_DIR}/External/glad/include
           ${CMAKE_SOURCE_DIR}/External/glm
           ${CMAKE_SOURCE_DIR}/External/spdlog/include
           ${CMAKE_SOURCE_DIR}/External/imgui
           ${CMAKE_SOURCE_DIR}/External/imgui/backends
           ${CMAKE_SOURCE_DIR}/External/stb_image
           ${CMAKE_SOURCE_DIR}/External/json/single_include
           ${CMAKE_SOURCE_DIR}/External/sol2/include
           ${CMAKE_SOURCE_DIR}/External/openal/include
           ${CMAKE_SOURCE_DIR}/External/audio
           ${CMAKE_SOURCE_DIR}/External/icons)

  target_compile_definitions(External PRIVATE GLFW_STATIC
                                              IMGUI_IMPL_OPENGL_LOADER_GLAD)

  target_link_libraries(External PUBLIC glfw)

  if(WIN32)
    target_link_libraries(External PRIVATE opengl32.lib winmm ole32 user32 uuid)
  elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(
      External
      PRIVATE OpenGL::GL
              ${X11_LIBRARIES}
              X11
              Xrandr
              Xi
              Xxf86vm
              Xcursor
              Xinerama
              dl
              pthread)
    message(STATUS "X11 libraries: ${X11_LIBRARIES}")
  elseif(APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(External PRIVATE OpenGL::GL)
  endif()
endif()
