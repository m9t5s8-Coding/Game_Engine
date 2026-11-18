# -----------------------------------------------------------------------------
# Configuration summary
# -----------------------------------------------------------------------------

message(STATUS "")
message(STATUS "========================================")
message(STATUS " Game Engine Configuration Summary")
message(STATUS "========================================")
message(STATUS " Platform    : ${PLATFORM_NAME}")
message(STATUS " C++ Standard: ${CMAKE_CXX_STANDARD}")

if(ANDROID)
  message(STATUS " Target      : Shared Library (libAeroRuntime.so)")
  message(STATUS " Graphics    : OpenGL ES 3.0")
  message(STATUS " Audio       : OpenAL-Soft (OpenSL ES backend)")
  message(STATUS " Editor      : not built for Android")
elseif(EMSCRIPTEN)
  message(STATUS " Target      : Sandbox.html (+ .js/.wasm, runtime paks)")
  message(STATUS " Graphics    : WebGL 2 / OpenGL ES 3.0")
  message(STATUS " Audio       : OpenAL (Emscripten)")
  message(STATUS " Window      : GLFW (Emscripten)")
  message(STATUS " Editor      : not built for WebAssembly")
else()
  set(_summary_targets)
  foreach(_summary_target AeroEditor Sandbox ServerSandbox AssetPacker)
    if(TARGET ${_summary_target})
      list(APPEND _summary_targets ${_summary_target})
    endif()
  endforeach()

  set(_summary_libs)
  foreach(_summary_lib ApplicationEditor ApplicationRuntime ApplicationServer)
    if(TARGET ${_summary_lib})
      list(APPEND _summary_libs ${_summary_lib})
    endif()
  endforeach()

  if(_summary_targets)
    message(STATUS " Targets     : ${_summary_targets}")
  else()
    message(STATUS " Targets     : (none — only libraries)")
  endif()
  if(_summary_libs)
    message(STATUS " Libraries   : ${_summary_libs}")
  endif()

  message(STATUS " Graphics    : OpenGL 4.5 (GLAD)")
  if(WIN32)
    message(STATUS " Audio       : OpenAL (WASAPI / DirectSound / WinMM)")
  elseif(UNIX AND NOT APPLE)
    message(STATUS " Audio       : OpenAL (PulseAudio / ALSA)")
  elseif(APPLE)
    message(STATUS " Audio       : OpenAL (CoreAudio)")
  endif()
  message(STATUS " Window      : GLFW")
  message(
    STATUS
      " Build Types : Debug | Release | RelWithDebInfo | MinSizeRel | Distributed"
  )
  if(WIN32)
    message(
      STATUS " Distributed : /MD dynamic runtime, LTO (/GL/LTCG), NSIS installer"
    )
  elseif(UNIX AND NOT APPLE)
    message(
      STATUS
        " Distributed : LTO (-flto), gc-sections, strip (-Wl,-s), DEB/RPM/TGZ")
  elseif(APPLE)
    message(
      STATUS " Distributed : LTO (-flto), dead_strip (-Wl,-dead_strip), .dmg")
  endif()
endif()

message(
  STATUS
    " Features    : editor=${BUILD_EDITOR} sandbox=${BUILD_SANDBOX} server=${BUILD_SERVER} tools=${BUILD_TOOLS}"
)
message(STATUS "========================================")
message(STATUS "")
