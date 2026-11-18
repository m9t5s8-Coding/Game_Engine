cmake_minimum_required(VERSION 3.20)

if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
      "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
  )
endif()

project("Game_Engine" LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# =============================================================================
# Platform detection, global flags and output directories
# =============================================================================
include(Platform)

# =============================================================================
# Build options
#
# Defaults keep the historical behaviour (build everything on desktop), while
# the presets in CMakePresets.json select smaller subsets. On Android and Web
# the platform decides what can be built, so the options are forced there.
# =============================================================================
option(BUILD_EDITOR "Build the AeroEditor application" ON)
option(BUILD_SANDBOX "Build the Sandbox runtime (desktop)" ON)
option(BUILD_SERVER "Build the ServerSandbox application" ON)
option(BUILD_TOOLS "Build command-line tools such as AssetPacker" ON)
option(AERO_PACK_ENGINE_ASSETS "Pack Application/assets into assets.pak" ON)

if(EMSCRIPTEN)
  set(BUILD_EDITOR
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_SERVER
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_TOOLS
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_SANDBOX
      ON
      CACHE BOOL "" FORCE)
elseif(ANDROID)
  set(BUILD_EDITOR
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_SERVER
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_TOOLS
      OFF
      CACHE BOOL "" FORCE)
  set(BUILD_SANDBOX
      ON
      CACHE BOOL "" FORCE)
endif()

# Application libraries are needed by every runtime; the editor and server
# libraries are added by Application/CMakeLists.txt only when requested.
set(AERO_BUILD_APP_LIBS OFF)
if(AERO_DESKTOP OR ANDROID OR EMSCRIPTEN)
  set(AERO_BUILD_APP_LIBS ON)
endif()

set(AERO_ASSETS_PAK "${CMAKE_BINARY_DIR}/assets.pak")
set(AERO_ENGINE_PAK
    "${CMAKE_SOURCE_DIR}/android/app/src/main/assets/assets.pak")
set(AERO_PROJECT_PAK
    "${CMAKE_SOURCE_DIR}/android/app/src/main/assets/game.pak")

set(AERO_ENGINE_ASSETS_ENABLED OFF)
if(AERO_DESKTOP
   AND AERO_PACK_ENGINE_ASSETS
   AND (BUILD_EDITOR OR BUILD_SANDBOX OR BUILD_SERVER OR BUILD_TOOLS))
  set(AERO_ENGINE_ASSETS_ENABLED ON)
endif()

# =============================================================================
# Third-party dependencies (GLFW, OpenAL, Lua, Box2D, ImGui, ...)
# =============================================================================
include(Dependencies)

# =============================================================================
# Tools are configured first so AeroEditor/Sandbox can depend on
# PackEngineAssets as soon as their targets are defined.
# =============================================================================
if(AERO_DESKTOP AND (BUILD_TOOLS OR AERO_ENGINE_ASSETS_ENABLED))
  # Use a dedicated binary directory: older build trees stored the AssetPacker
  # executable at <build>/tools/AssetPacker, which cannot coexist with a
  # generated subdirectory of the same name.
  add_subdirectory(tools/AssetPacker "${CMAKE_BINARY_DIR}/tools-build/AssetPacker")
endif()

# =============================================================================
# Engine libraries + applications
# =============================================================================
if(AERO_BUILD_APP_LIBS)
  add_subdirectory(Application)
endif()

add_subdirectory(Sandbox)
add_subdirectory(AeroEditor)
add_subdirectory(ServerSandbox)

# =============================================================================
# Installation / CPack
# =============================================================================
include(Packaging)

# =============================================================================
# Configuration summary
# =============================================================================
include(Summary)
