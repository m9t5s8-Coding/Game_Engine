# -----------------------------------------------------------------------------
# Sandbox runtime
#
# Desktop  -> executable "Sandbox"
# Web      -> executable "Sandbox.html" (.js/.wasm + runtime-fetched paks)
# Android  -> shared library "libAeroRuntime.so" loaded by the Java app
# -----------------------------------------------------------------------------

if(ANDROID)
  message(STATUS "Configuring Android runtime...")

  add_library(
    native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
  target_include_directories(
    native_app_glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)

  file(GLOB_RECURSE SANDBOX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

  add_library(
    AeroRuntime SHARED
    ${CMAKE_SOURCE_DIR}/Application/src/Platform/Android/android_main.cpp
    ${SANDBOX_SRC})

  target_include_directories(
    AeroRuntime
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/Application/include
            ${ANDROID_NDK}/sources/android/native_app_glue)

  target_compile_definitions(AeroRuntime PRIVATE AERO_RUNTIME)

  target_link_libraries(
    AeroRuntime
    PRIVATE ApplicationRuntime
            -Wl,--whole-archive
            native_app_glue
            -Wl,--no-whole-archive
            ${log-lib}
            ${android-lib}
            ${EGL-lib}
            ${GLESv3-lib}
            ${OpenSLES-lib})

elseif(EMSCRIPTEN)
  message(STATUS "Creating WebAssembly executable target...")

  file(GLOB_RECURSE SANDBOX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
  add_executable(Sandbox ${SANDBOX_SRC})
  set_msvc_runtime_library(Sandbox)
  target_include_directories(Sandbox
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(Sandbox PRIVATE ApplicationRuntime)
  target_compile_definitions(Sandbox PRIVATE AERO_RUNTIME)

  set_target_properties(
    Sandbox
    PROPERTIES CXX_STANDARD 20
               CXX_STANDARD_REQUIRED ON
               CXX_EXTENSIONS OFF
               SUFFIX ".html"
               RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

  target_precompile_headers(Sandbox PRIVATE
                            "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp")

  # Prefer the already-packed Android assets when they are available; the paks
  # are fetched at runtime (see web/preload_paks.js) instead of being baked
  # into Sandbox.data. If no engine pak exists, fall back to baking the loose
  # assets folder so the build still runs.
  set(WEB_PRELOAD_FILES)
  set(WEB_RUNTIME_PAKS)

  if(EXISTS "${AERO_ENGINE_PAK}")
    list(APPEND WEB_RUNTIME_PAKS "${AERO_ENGINE_PAK}")
  else()
    list(APPEND WEB_PRELOAD_FILES
         "${CMAKE_SOURCE_DIR}/Application/assets@/assets")
  endif()

  if(EXISTS "${AERO_PROJECT_PAK}")
    list(APPEND WEB_RUNTIME_PAKS "${AERO_PROJECT_PAK}")
  endif()

  set(WEB_PRELOAD_OPTIONS)
  set(WEB_PRELOAD_DEPENDS)
  foreach(_file IN LISTS WEB_PRELOAD_FILES)
    list(APPEND WEB_PRELOAD_OPTIONS "--preload-file=${_file}")

    # --preload-file paths are evaluated by the linker but are not tracked as
    # inputs by Ninja/Make. Add them (and the contents of preloaded folders) as
    # link dependencies so replacing the loose assets regenerates Sandbox.data
    # without a clean build.
    string(REGEX REPLACE "@.*$" "" _preload_src "${_file}")
    if(IS_DIRECTORY "${_preload_src}")
      file(GLOB_RECURSE _preload_dir_files "${_preload_src}/*")
      list(APPEND WEB_PRELOAD_DEPENDS ${_preload_dir_files})
    else()
      list(APPEND WEB_PRELOAD_DEPENDS "${_preload_src}")
    endif()
  endforeach()

  if(WEB_PRELOAD_DEPENDS)
    set_property(TARGET Sandbox APPEND PROPERTY LINK_DEPENDS
                                               ${WEB_PRELOAD_DEPENDS})
  endif()

  # Put the runtime-fetched paks next to Sandbox.html on the first build.
  # Replacing them there later needs no rebuild at all.
  foreach(_pak IN LISTS WEB_RUNTIME_PAKS)
    get_filename_component(_pak_name "${_pak}" NAME)
    add_custom_command(
      TARGET Sandbox
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_pak}"
              "$<TARGET_FILE_DIR:Sandbox>/${_pak_name}"
      COMMENT "Copying ${_pak_name} to the web output directory")
  endforeach()

  target_link_options(
    Sandbox
    PRIVATE "-sUSE_GLFW=3"
            "-sUSE_WEBGL2=1"
            "-sFULL_ES3=1"
            "-sALLOW_MEMORY_GROWTH=1"
            "-sINITIAL_MEMORY=67108864"
            "-sSTACK_SIZE=8388608"
            "-sFORCE_FILESYSTEM=1"
            "--pre-js=${CMAKE_SOURCE_DIR}/web/preload_paks.js"
            ${WEB_PRELOAD_OPTIONS})

  message(STATUS "Web runtime paks: ${WEB_RUNTIME_PAKS}")
  message(STATUS "Web preload files: ${WEB_PRELOAD_FILES}")

elseif(BUILD_SANDBOX)
  file(GLOB_RECURSE SANDBOX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
  add_executable(Sandbox ${SANDBOX_SRC})
  set_msvc_runtime_library(Sandbox)
  target_include_directories(Sandbox
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(Sandbox PRIVATE ApplicationRuntime)
  target_compile_definitions(
    Sandbox PRIVATE AERO_RUNTIME $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>)
  if(WIN32)
    target_link_options(
      Sandbox PRIVATE "$<$<CONFIG:Distributed>:/SUBSYSTEM:WINDOWS>"
      "$<$<CONFIG:Distributed>:/ENTRY:mainCRTStartup>")
  endif()
  target_precompile_headers(Sandbox PRIVATE
                            "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp")

  # A desktop Sandbox needs assets.pak in its working/output directory and
  # game.pak next to the executable.
  if(TARGET PackEngineAssets)
    add_dependencies(Sandbox PackEngineAssets)
    add_custom_command(
      TARGET Sandbox
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AERO_ASSETS_PAK}"
              "$<TARGET_FILE_DIR:Sandbox>/assets.pak"
      COMMENT "Copying assets.pak to the Sandbox output directory")
  endif()

  if(EXISTS "${AERO_PROJECT_PAK}")
    add_custom_command(
      TARGET Sandbox
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AERO_PROJECT_PAK}"
              "$<TARGET_FILE_DIR:Sandbox>/game.pak"
      COMMENT "Copying game.pak to the Sandbox output directory")
  endif()

  aero_copy_openal_dll(Sandbox)
endif()
