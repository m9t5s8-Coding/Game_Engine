# -----------------------------------------------------------------------------
# Platform detection and global build settings
# -----------------------------------------------------------------------------

if(EMSCRIPTEN OR CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set(PLATFORM_NAME "Web")
  set(EMSCRIPTEN
      ON
      CACHE BOOL "Build for WebAssembly with Emscripten" FORCE)
  add_definitions(-DPLATFORM_WEB)
  message(STATUS "Building for WebAssembly (Emscripten)")
elseif(ANDROID)
  set(PLATFORM_NAME "Android")
  add_definitions(-DPLATFORM_ANDROID)
  message(STATUS "Building for Android")
elseif(WIN32)
  set(PLATFORM_NAME "Windows")
  add_definitions(-DPLATFORM_WINDOWS)
  message(STATUS "Building for Windows")
elseif(APPLE)
  set(PLATFORM_NAME "macOS")
  add_definitions(-DPLATFORM_MACOS)
  message(STATUS "Building for macOS")
elseif(UNIX)
  set(PLATFORM_NAME "Linux")
  add_definitions(-DPLATFORM_LINUX)
  message(STATUS "Building for Linux")
endif()

set(AERO_DESKTOP OFF)
if(NOT ANDROID AND NOT EMSCRIPTEN)
  set(AERO_DESKTOP ON)
endif()

if(MSVC)
  add_compile_options(/utf-8)
  add_compile_options(/bigobj)
endif()

if(NOT ANDROID)
  get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

  if(_is_multi_config)
    set(CMAKE_CONFIGURATION_TYPES
        "Debug;Release;RelWithDebInfo;MinSizeRel;Distributed"
        CACHE STRING "Available build configurations" FORCE)
    message(STATUS "Multi-config generator detected: all configs available")
  else()
    if(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE
          "Debug"
          CACHE STRING "Choose build type" FORCE)
    endif()
    set_property(
      CACHE CMAKE_BUILD_TYPE
      PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel"
               "Distributed")
    message(
      STATUS
        "Single-config generator detected: CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}"
    )
  endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =============================================================================
# Runtime linking - platform specific
# =============================================================================

if(MSVC)
  # Windows: all configurations use /MD (dynamic runtime)
  set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_C_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/MD /O1 /Ob1 /D NDEBUG")
  set(CMAKE_C_FLAGS_MINSIZEREL "/MD /O1 /Ob1 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_DISTRIBUTED "/MD /O2 /Ob2 /D NDEBUG /GL /Gy")
  set(CMAKE_C_FLAGS_DISTRIBUTED "/MD /O2 /Ob2 /D NDEBUG /GL /Gy")
  set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTED
      "/INCREMENTAL:NO /LTCG /OPT:REF /OPT:ICF")
  set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTED
      "/INCREMENTAL:NO /LTCG /OPT:REF /OPT:ICF")
  set(CMAKE_STATIC_LINKER_FLAGS_DISTRIBUTED "/LTCG")
  message(STATUS "MSVC: All configurations using dynamic runtime (/MD)")

elseif(UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
  set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
  set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
  # Distributed: maximum optimization + LTO + strip + dynamic runtime
  set(CMAKE_CXX_FLAGS_DISTRIBUTED
      "-O3 -DNDEBUG -flto -ffunction-sections -fdata-sections -fvisibility=hidden -shared-libstdc++"
  )
  set(CMAKE_C_FLAGS_DISTRIBUTED
      "-O3 -DNDEBUG -flto -ffunction-sections -fdata-sections -fvisibility=hidden"
  )
  set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTED
      "-flto -Wl,--gc-sections -Wl,-s -shared-libstdc++ -Wl,--as-needed")
  set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTED
      "-flto -Wl,--gc-sections -Wl,-s -shared-libstdc++ -Wl,--as-needed")
  set(CMAKE_STATIC_LINKER_FLAGS_DISTRIBUTED "")
  message(
    STATUS
      "GCC/Clang: Using shared libstdc++ (dynamic runtime) with LTO + strip + gc-sections"
  )

elseif(APPLE)
  # Clang on macOS
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
  set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
  set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
  # Distributed: LTO + dead strip (macOS linker uses -dead_strip)
  set(CMAKE_CXX_FLAGS_DISTRIBUTED
      "-O3 -DNDEBUG -flto -ffunction-sections -fdata-sections -fvisibility=hidden"
  )
  set(CMAKE_C_FLAGS_DISTRIBUTED
      "-O3 -DNDEBUG -flto -ffunction-sections -fdata-sections -fvisibility=hidden"
  )
  set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTED "-flto -Wl,-dead_strip -Wl,-S")
  set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTED "-flto -Wl,-dead_strip -Wl,-S")
  set(CMAKE_STATIC_LINKER_FLAGS_DISTRIBUTED "")
  message(
    STATUS "Apple Clang: Distributed configuration will use LTO + dead_strip")
endif()

if(NOT ANDROID)
  if(_is_multi_config)
    foreach(config IN ITEMS Debug Release RelWithDebInfo MinSizeRel Distributed)
      string(TOUPPER ${config} config_upper)
      set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper}
          "${CMAKE_BINARY_DIR}/${config}/bin")
      set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper}
          "${CMAKE_BINARY_DIR}/${config}/lib")
      set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper}
          "${CMAKE_BINARY_DIR}/${config}/lib")
      set(CMAKE_PDB_OUTPUT_DIRECTORY_${config_upper}
          "${CMAKE_BINARY_DIR}/${config}/pdb")
    endforeach()
  else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pdb")
  endif()
endif()

function(set_msvc_runtime_library target)
  if(MSVC)
    set_target_properties(
      ${target}
      PROPERTIES
        MSVC_RUNTIME_LIBRARY
        "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>\
$<$<CONFIG:Release>:MultiThreadedDLL>\
$<$<CONFIG:RelWithDebInfo>:MultiThreadedDLL>\
$<$<CONFIG:MinSizeRel>:MultiThreadedDLL>\
$<$<CONFIG:Distributed>:MultiThreadedDLL>")
  endif()
endfunction()

# Copy the OpenAL DLL next to a Windows executable when OpenAL is shared.
function(aero_copy_openal_dll target)
  if(NOT WIN32 OR NOT TARGET ${target} OR NOT TARGET OpenAL)
    return()
  endif()

  get_target_property(_openal_type OpenAL TYPE)
  if(_openal_type STREQUAL "SHARED_LIBRARY")
    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:OpenAL>"
              "$<TARGET_FILE_DIR:${target}>"
      COMMENT "Copying OpenAL DLL -> ${target}"
      VERBATIM)
  endif()
endfunction()
