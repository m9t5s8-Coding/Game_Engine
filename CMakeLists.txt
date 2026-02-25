cmake_minimum_required(VERSION 3.20)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("Game_Engine" LANGUAGES C CXX)


# Platform Detection
if(ANDROID)
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


if(MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/bigobj)
endif()





set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(NOT ANDROID)
    message(STATUS "Configuring desktop libraries...")

    # GLFW
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw)

    # OpenAL - Platform-specific backend configuration
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)

set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS    OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS    OFF CACHE BOOL "" FORCE)
set(ALSOFT_CONFIG   OFF CACHE BOOL "" FORCE)

    # Disable mobile backends
    set(ALSOFT_REQUIRE_OPENSL OFF CACHE BOOL "" FORCE)

    if(WIN32)
        # Windows audio backends
        set(ALSOFT_REQUIRE_WINMM ON CACHE BOOL "" FORCE)
        set(ALSOFT_REQUIRE_DSOUND ON CACHE BOOL "" FORCE)
        set(ALSOFT_REQUIRE_WASAPI ON CACHE BOOL "" FORCE)

        set(ALSOFT_BACKEND_WINMM ON CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_DSOUND ON CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WASAPI ON CACHE BOOL "" FORCE)

        # Disable Linux/Mac backends on Windows
        set(ALSOFT_BACKEND_ALSA OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_PULSEAUDIO OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_COREAUDIO OFF CACHE BOOL "" FORCE)
    elseif(UNIX AND NOT APPLE)
        # Linux audio backends
        set(ALSOFT_BACKEND_ALSA ON CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_PULSEAUDIO ON CACHE BOOL "" FORCE)

        # Disable Windows/Mac backends on Linux
        set(ALSOFT_BACKEND_WINMM OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_DSOUND OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WASAPI OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_COREAUDIO OFF CACHE BOOL "" FORCE)
    elseif(APPLE)
        # macOS audio backend
        set(ALSOFT_BACKEND_COREAUDIO ON CACHE BOOL "" FORCE)

        # Disable Windows/Linux backends on macOS
        set(ALSOFT_BACKEND_WINMM OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_DSOUND OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WASAPI OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_ALSA OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_PULSEAUDIO OFF CACHE BOOL "" FORCE)
    endif()

    # Common disabled backends
    set(ALSOFT_BACKEND_PORTAUDIO OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_OSS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_SNDIO OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_SOLARIS OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_SOUNDIO OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_WAVE OFF CACHE BOOL "" FORCE)

    set(LIBTYPE "STATIC" CACHE STRING "Library type" FORCE)

    add_subdirectory(${CMAKE_SOURCE_DIR}/External/openal)
endif()


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
        "${CMAKE_SOURCE_DIR}/External/lua/lzio.c"
)

add_library(lua STATIC ${LUA_SRC})
target_include_directories(lua PUBLIC "${CMAKE_SOURCE_DIR}/External/lua")

if(ANDROID)
    target_compile_definitions(lua PRIVATE
            LUA_COMPAT_5_3
            LUA_USE_POSIX
    )
elseif(UNIX)
    # Linux/macOS
    target_compile_definitions(lua PRIVATE
            LUA_COMPAT_5_3
            LUA_USE_POSIX
            LUA_USE_DLOPEN
    )
    # Link libdl for dynamic loading on Linux
    if(NOT APPLE)
        target_link_libraries(lua PUBLIC dl)
    endif()
else()
    # Windows
    target_compile_definitions(lua PRIVATE
            LUA_COMPAT_5_3
            _CRT_SECURE_NO_WARNINGS
    )
endif()


file(GLOB_RECURSE BOX2D_SRC "${CMAKE_SOURCE_DIR}/External/box2d/src/*.cpp")
add_library(box2d STATIC ${BOX2D_SRC})
target_include_directories(box2d PUBLIC "${CMAKE_SOURCE_DIR}/External/box2d/include")


set(IMGUI_CORE_SRC
        "${CMAKE_SOURCE_DIR}/External/imgui/imgui.cpp"
        "${CMAKE_SOURCE_DIR}/External/imgui/imgui_demo.cpp"
        "${CMAKE_SOURCE_DIR}/External/imgui/imgui_draw.cpp"
        "${CMAKE_SOURCE_DIR}/External/imgui/imgui_widgets.cpp"
        "${CMAKE_SOURCE_DIR}/External/imgui/imgui_tables.cpp"
)

if(ANDROID)
    # Android: Android backend + OpenGL ES
    set(IMGUI_BACKEND_SRC
            "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_android.cpp"
            "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp"
    )
else()
    # Desktop: GLFW backend + OpenGL (with GLAD)
    set(IMGUI_BACKEND_SRC
            "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_glfw.cpp"
            "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp"
    )
endif()

add_library(imgui STATIC ${IMGUI_CORE_SRC} ${IMGUI_BACKEND_SRC})

target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/External/imgui
        ${CMAKE_SOURCE_DIR}/External/imgui/backends
)

if(NOT ANDROID)
    target_include_directories(imgui PUBLIC
            ${CMAKE_SOURCE_DIR}/External/glfw/include
    )
endif()


if(ANDROID)
    target_compile_definitions(imgui PRIVATE
            IMGUI_IMPL_OPENGL_ES3
    )
else()
    target_compile_definitions(imgui PRIVATE
            IMGUI_IMPL_OPENGL_LOADER_GLAD
    )
endif()


if(ANDROID)
    add_library(External INTERFACE)

    target_include_directories(External INTERFACE
            ${CMAKE_SOURCE_DIR}/External/entt/include
            ${CMAKE_SOURCE_DIR}/External/glm
            ${CMAKE_SOURCE_DIR}/External/spdlog/include
            ${CMAKE_SOURCE_DIR}/External/imgui
            ${CMAKE_SOURCE_DIR}/External/imgui/backends
            ${CMAKE_SOURCE_DIR}/External/stb_image
            ${CMAKE_SOURCE_DIR}/External/json/single_include
            ${CMAKE_SOURCE_DIR}/External/sol2/include
            ${CMAKE_SOURCE_DIR}/External/audio
            ${CMAKE_SOURCE_DIR}/External/icons
    )

else()

    set(GLAD_SRC "${CMAKE_SOURCE_DIR}/External/glad/glad.cpp")
    add_library(External STATIC ${GLAD_SRC})

    target_include_directories(External PUBLIC
            ${CMAKE_SOURCE_DIR}/External/entt/include
            ${CMAKE_SOURCE_DIR}/External/glfw/include
            ${CMAKE_SOURCE_DIR}/External/glad/include
            ${CMAKE_SOURCE_DIR}/External/glm
            ${CMAKE_SOURCE_DIR}/External/spdlog/include
            ${CMAKE_SOURCE_DIR}/External/imgui
            ${CMAKE_SOURCE_DIR}/External/imgui/backends
            ${CMAKE_SOURCE_DIR}/External/stb_image
            ${CMAKE_SOURCE_DIR}/External/json/single_include
            ${CMAKE_SOURCE_DIR}/External/sol2/include
            ${CMAKE_SOURCE_DIR}/External/audio
            ${CMAKE_SOURCE_DIR}/External/icons
    )

    target_compile_definitions(External PRIVATE
            GLFW_STATIC
            IMGUI_IMPL_OPENGL_LOADER_GLAD
    )

    target_link_libraries(External PUBLIC glfw)

    # Platform-specific OpenGL linking
    if(WIN32)
        target_link_libraries(External PRIVATE opengl32.lib)
    elseif(UNIX AND NOT APPLE)
        # Linux: Link OpenGL and required X11 libraries
        find_package(OpenGL REQUIRED)
        find_package(X11 REQUIRED)

        target_link_libraries(External PRIVATE
                OpenGL::GL
                ${X11_LIBRARIES}
                X11
                Xrandr
                Xi
                Xxf86vm
                Xcursor
                Xinerama
                dl
                pthread
        )

        message(STATUS "X11 libraries: ${X11_LIBRARIES}")
    elseif(APPLE)
        # macOS: Link OpenGL framework
        find_package(OpenGL REQUIRED)
        target_link_libraries(External PRIVATE OpenGL::GL)
    endif()

    # Windows-specific libraries
    if(WIN32)
        target_link_libraries(External PRIVATE
                winmm
                ole32
                user32
                uuid
        )
    endif()
endif()


file(GLOB_RECURSE APP_CORE_SRC
        "${CMAKE_SOURCE_DIR}/Application/src/Audio/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/Core/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/GameObjects/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/ImGui/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/Layers/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/Project/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/Renderer/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/Scene/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/Scripting/*.cpp"
)


if(ANDROID)
    file(GLOB APP_PLATFORM_SRC
            "${CMAKE_SOURCE_DIR}/Application/src/Platform/Android/*.cpp"
    )
else()
    file(GLOB APP_PLATFORM_SRC
            "${CMAKE_SOURCE_DIR}/Application/src/Platform/WindowsWindow/*.cpp"
            "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenGL/*.cpp"
            "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenAL/*.cpp"
    )
endif()


set(APP_SRC ${APP_CORE_SRC} ${APP_PLATFORM_SRC})


if(NOT APP_PLATFORM_SRC)
    message(WARNING "No platform-specific sources found, using all sources")
    file(GLOB_RECURSE APP_SRC "${CMAKE_SOURCE_DIR}/Application/src/*.cpp")
endif()

add_library(Application STATIC ${APP_SRC})

target_include_directories(Application PUBLIC
        ${CMAKE_SOURCE_DIR}/Application/include
        ${CMAKE_SOURCE_DIR}/External/box2d/include
        ${CMAKE_SOURCE_DIR}/External/lua
        ${CMAKE_SOURCE_DIR}/External/imgui
        ${CMAKE_SOURCE_DIR}/External/imgui/backends
)

target_link_libraries(Application PUBLIC
        lua
        box2d
        imgui
)


if(ANDROID)
    target_link_libraries(Application PUBLIC External)
else()
    target_link_libraries(Application PUBLIC External)
endif()

target_compile_definitions(Application PRIVATE
        STB_IMAGE_IMPLEMENTATION
        DR_MP3_IMPLEMENTATION
        DR_WAV_IMPLEMENTATION
)

if(UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    # Add GTK3 to Application target includes and libs
    target_include_directories(Application PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(Application PRIVATE ${GTK3_LIBRARIES})
    target_compile_options(Application PRIVATE ${GTK3_CFLAGS_OTHER})

    message(STATUS "GTK3 found: ${GTK3_VERSION}")
endif()


if(ANDROID)
    message(STATUS "Configuring Android libraries for Application...")

    find_library(log-lib log)
    find_library(android-lib android)
    find_library(EGL-lib EGL)
    find_library(GLESv3-lib GLESv3)
    find_library(OpenSLES-lib OpenSLES)

    target_link_libraries(Application PUBLIC
            ${log-lib}
            ${android-lib}
            ${EGL-lib}
            ${GLESv3-lib}
            ${OpenSLES-lib}
    )

    target_compile_definitions(Application PUBLIC
            IMGUI_IMPL_OPENGL_ES3
    )

else()

    target_link_libraries(Application PUBLIC OpenAL)

    if(WIN32)
        target_link_libraries(Application PRIVATE
                user32.lib
                gdi32.lib
                shell32.lib
        )
    elseif(UNIX AND NOT APPLE)
        # Linux: Link pthread and math library
        target_link_libraries(Application PRIVATE
                pthread
                m
        )
    endif()
endif()


if(ANDROID)
    message(STATUS "Creating Android shared library target...")

    # First, create the native_app_glue library
    add_library(native_app_glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    )

    target_include_directories(native_app_glue PUBLIC
            ${ANDROID_NDK}/sources/android/native_app_glue
    )

    # Now create your main library
    add_library(aeroengine SHARED
            ${CMAKE_SOURCE_DIR}/Application/src/Platform/Android/android_main.cpp
    )

    target_include_directories(aeroengine PRIVATE
            ${CMAKE_SOURCE_DIR}/Application/include
    )

    target_link_libraries(aeroengine PRIVATE
            Application
            native_app_glue
            ${log-lib}
            ${android-lib}
    )

else()
    message(STATUS "Creating desktop executable targets...")

    file(GLOB_RECURSE AERO_SRC "${CMAKE_SOURCE_DIR}/AeroEditor/src/*.cpp")
    add_executable(AeroEditor ${AERO_SRC})

    if(WIN32)
        target_link_libraries(AeroEditor PRIVATE Shell32 Ole32)
    elseif(UNIX AND NOT APPLE)
        # Linux: Link standard libraries (X11 already linked via External)
        target_link_libraries(AeroEditor PRIVATE pthread dl)
    endif()

    target_include_directories(AeroEditor PRIVATE
            ${CMAKE_SOURCE_DIR}/AeroEditor/include
    )

    target_link_libraries(AeroEditor PRIVATE Application)



    file(GLOB_RECURSE SANDBOX_SRC "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp")
    add_executable(Sandbox ${SANDBOX_SRC})

    target_include_directories(Sandbox PRIVATE
            ${CMAKE_SOURCE_DIR}/Sandbox/include
    )

    target_link_libraries(Sandbox PRIVATE Application)





    target_precompile_headers(Application PUBLIC
            "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
    )

    target_precompile_headers(AeroEditor PRIVATE
            "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
    )

    target_precompile_headers(Sandbox PRIVATE
            "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
    )


    add_executable(AssetPacker ${CMAKE_SOURCE_DIR}/tools/AssetPacker/main.cpp)
    target_include_directories(AssetPacker PRIVATE ${CMAKE_SOURCE_DIR}/Application/include)
    target_link_libraries(AssetPacker PRIVATE Application)
    target_precompile_headers(AssetPacker PRIVATE "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp")
set_target_properties(AssetPacker PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

file(GLOB_RECURSE ENGINE_ASSET_FILES
    "${CMAKE_SOURCE_DIR}/Application/assets/*"
)

set(PAK_OUTPUT "${CMAKE_BINARY_DIR}/assets.pak")

# Re-pack whenever any asset file changes
add_custom_command(
    OUTPUT  "${PAK_OUTPUT}"
    COMMAND AssetPacker
            "${CMAKE_SOURCE_DIR}/Application/assets"
            "${PAK_OUTPUT}"
    DEPENDS AssetPacker ${ENGINE_ASSET_FILES}
    COMMENT "Packing engine assets -> assets.pak"
)

add_custom_target(PackEngineAssets ALL
    DEPENDS "${PAK_OUTPUT}"
)

# AeroEditor must wait for packing to finish
add_dependencies(AeroEditor PackEngineAssets)

# Copy the .pak next to AeroEditor.exe after build
add_custom_command(TARGET AeroEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PAK_OUTPUT}"
        "$<TARGET_FILE_DIR:AeroEditor>/assets.pak"
    COMMENT "Copying assets.pak to output dir"
)


get_target_property(_openal_type OpenAL TYPE)
    get_target_property(_openal_includes OpenAL INTERFACE_INCLUDE_DIRECTORIES)

    message(STATUS "OpenAL TYPE = ${_openal_type}")
    message(STATUS "OpenAL INCLUDES = ${_openal_includes}")
endif()

message(STATUS "")
message(STATUS "Game Engine Configuration Summary")
message(STATUS "Platform: ${PLATFORM_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

if(ANDROID)
    message(STATUS "Target: Shared Library (libaeroengine.so)")
    message(STATUS "Graphics: OpenGL ES 3.0")
    message(STATUS "Audio: OpenSL ES")
else()
    message(STATUS "Targets: AeroEditor, Sandbox")
    message(STATUS "Graphics: OpenGL 4.5 (GLAD)")

    if(WIN32)
        message(STATUS "Audio: OpenAL (WASAPI/DirectSound/WinMM)")
    elseif(UNIX AND NOT APPLE)
        message(STATUS "Audio: OpenAL (PulseAudio/ALSA)")
    elseif(APPLE)
        message(STATUS "Audio: OpenAL (CoreAudio)")
    endif()

    message(STATUS "Window: GLFW")
endif()

message(STATUS "")
