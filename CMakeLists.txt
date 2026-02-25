cmake_minimum_required(VERSION 3.20)

# Set policies early
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("Game_Engine" LANGUAGES C CXX)

# ============================================
# Platform Detection
# ============================================
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

# ============================================
# Build Configurations (Desktop only)
# ============================================
if(NOT ANDROID)
    get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

    if(_is_multi_config)

        set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;Distributed"
            CACHE STRING "Available build configurations" FORCE)
        message(STATUS "Multi-config generator detected: all configs available")

    else()

        if(NOT CMAKE_BUILD_TYPE)
            set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose build type" FORCE)
        endif()
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "RelWithDebInfo" "MinSizeRel" "Distributed")
        message(STATUS "Single-config generator detected: CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================
# Compiler Flags per Build Type
# ============================================
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG           "/MDd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_C_FLAGS_DEBUG             "/MDd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE         "/MD /O2 /Ob2 /D NDEBUG")
    set(CMAKE_C_FLAGS_RELEASE           "/MD /O2 /Ob2 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "/MD /Zi /O2 /Ob1 /D NDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO    "/MD /Zi /O2 /Ob1 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL      "/MD /O1 /Ob1 /D NDEBUG")
    set(CMAKE_C_FLAGS_MINSIZEREL        "/MD /O1 /Ob1 /D NDEBUG")
    # Distributed: static runtime + full optimization + LTO
    set(CMAKE_CXX_FLAGS_DISTRIBUTED     "/MT /O2 /Ob2 /D NDEBUG /GL /Gy")
    set(CMAKE_C_FLAGS_DISTRIBUTED       "/MT /O2 /Ob2 /D NDEBUG /GL /Gy")
    set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTED    "/INCREMENTAL:NO /LTCG /OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTED "/INCREMENTAL:NO /LTCG /OPT:REF /OPT:ICF")
    set(CMAKE_STATIC_LINKER_FLAGS_DISTRIBUTED "/LTCG")
    message(STATUS "MSVC: Distributed configuration will use /MT (static runtime)")

elseif(UNIX AND NOT APPLE)
    # GCC / Clang on Linux
    set(CMAKE_CXX_FLAGS_DEBUG           "-g -O0 -DDEBUG")
    set(CMAKE_C_FLAGS_DEBUG             "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE         "-O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE           "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO    "-O2 -g -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
    set(CMAKE_C_FLAGS_MINSIZEREL        "-Os -DNDEBUG")
    # Distributed: maximum optimization + LTO + strip symbols
    set(CMAKE_CXX_FLAGS_DISTRIBUTED     "-O3 -DNDEBUG -flto -ffunction-sections -fdata-sections -fvisibility=hidden")
    set(CMAKE_C_FLAGS_DISTRIBUTED       "-O3 -DNDEBUG -flto -ffunction-sections -fdata-sections -fvisibility=hidden")
    set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTED    "-flto -Wl,--gc-sections -Wl,-s")
    set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTED "-flto -Wl,--gc-sections -Wl,-s")
    set(CMAKE_STATIC_LINKER_FLAGS_DISTRIBUTED "")
    message(STATUS "GCC/Clang: Distributed configuration will use LTO + strip + gc-sections")

elseif(APPLE)
    # Clang on macOS
    set(CMAKE_CXX_FLAGS_DEBUG           "-g -O0 -DDEBUG")
    set(CMAKE_C_FLAGS_DEBUG             "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE         "-O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE           "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO    "-O2 -g -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
    set(CMAKE_C_FLAGS_MINSIZEREL        "-Os -DNDEBUG")
    # Distributed: LTO + dead strip (macOS linker uses -dead_strip instead of --gc-sections)
    set(CMAKE_CXX_FLAGS_DISTRIBUTED     "-O3 -DNDEBUG -flto -ffunction-sections -fdata-sections -fvisibility=hidden")
    set(CMAKE_C_FLAGS_DISTRIBUTED       "-O3 -DNDEBUG -flto -ffunction-sections -fdata-sections -fvisibility=hidden")
    set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTED    "-flto -Wl,-dead_strip -Wl,-S")
    set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTED "-flto -Wl,-dead_strip -Wl,-S")
    set(CMAKE_STATIC_LINKER_FLAGS_DISTRIBUTED "")
    message(STATUS "Apple Clang: Distributed configuration will use LTO + dead_strip")
endif()

# ============================================
# Output Directories
# ============================================
if(NOT ANDROID)
    if(CMAKE_CONFIGURATION_TYPES)
        foreach(config IN ITEMS Debug Release RelWithDebInfo MinSizeRel Distributed)
            string(TOUPPER ${config} config_upper)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/${config}/bin")
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/${config}/lib")
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/${config}/lib")
            set(CMAKE_PDB_OUTPUT_DIRECTORY_${config_upper}     "${CMAKE_BINARY_DIR}/${config}/pdb")
        endforeach()
    else()
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
        set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/pdb")
    endif()
endif()

# ============================================
# Helper: Set MSVC Runtime Library
# ============================================
function(set_msvc_runtime_library target)
    if(MSVC)
        set_target_properties(${target} PROPERTIES
            MSVC_RUNTIME_LIBRARY
            "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>\
$<$<CONFIG:Release>:MultiThreadedDLL>\
$<$<CONFIG:RelWithDebInfo>:MultiThreadedDLL>\
$<$<CONFIG:MinSizeRel>:MultiThreadedDLL>\
$<$<CONFIG:Distributed>:MultiThreaded>"
        )
    endif()
endfunction()

# ============================================
# Desktop-only External Libraries
# ============================================
if(NOT ANDROID)
    message(STATUS "Configuring desktop libraries...")

    # --- GLFW (static) ---
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw)
    set_msvc_runtime_library(glfw)

    # --- OpenAL-Soft ---
    # Distributed (MSVC): static + /MT   |   Everything else: static + matching runtime
    set(ALSOFT_UTILS                    OFF CACHE BOOL "" FORCE)
    set(ALSOFT_NO_CONFIG_UTIL           OFF CACHE BOOL "" FORCE)
    set(ALSOFT_EXAMPLES                 OFF CACHE BOOL "" FORCE)
    set(ALSOFT_TESTS                    OFF CACHE BOOL "" FORCE)
    set(ALSOFT_CONFIG                   OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL                  OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_CONFIG           OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_HRTF_DATA        OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_AMBDEC_PRESETS   OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_EXAMPLES         OFF CACHE BOOL "" FORCE)
    set(ALSOFT_INSTALL_UTILS            OFF CACHE BOOL "" FORCE)
    set(ALSOFT_REQUIRE_OPENSL           OFF CACHE BOOL "" FORCE)

    # Platform-specific audio backends
    if(WIN32)
        set(ALSOFT_REQUIRE_WINMM    ON  CACHE BOOL "" FORCE)
        set(ALSOFT_REQUIRE_DSOUND   ON  CACHE BOOL "" FORCE)
        set(ALSOFT_REQUIRE_WASAPI   ON  CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WINMM    ON  CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_DSOUND   ON  CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WASAPI   ON  CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_ALSA         OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_PULSEAUDIO   OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_COREAUDIO    OFF CACHE BOOL "" FORCE)
    elseif(UNIX AND NOT APPLE)
        set(ALSOFT_BACKEND_ALSA         ON  CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_PULSEAUDIO   ON  CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WINMM        OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_DSOUND       OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WASAPI       OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_COREAUDIO    OFF CACHE BOOL "" FORCE)
    elseif(APPLE)
        set(ALSOFT_BACKEND_COREAUDIO    ON  CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WINMM        OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_DSOUND       OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_WASAPI       OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_ALSA         OFF CACHE BOOL "" FORCE)
        set(ALSOFT_BACKEND_PULSEAUDIO   OFF CACHE BOOL "" FORCE)
    endif()

    # Commonly disabled backends
    set(ALSOFT_BACKEND_PORTAUDIO    OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_OSS          OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_SNDIO        OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_SOLARIS      OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_SOUNDIO      OFF CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_WAVE         OFF CACHE BOOL "" FORCE)

    # Always build static
    set(LIBTYPE "STATIC" CACHE STRING "Library type" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


if(MSVC)
    add_subdirectory(${CMAKE_SOURCE_DIR}/External/openal)

    get_directory_property(openal_targets
        DIRECTORY ${CMAKE_SOURCE_DIR}/External/openal BUILDSYSTEM_TARGETS)
    foreach(tgt ${openal_targets})
        if(TARGET ${tgt})
            get_target_property(tgt_type ${tgt} TYPE)
            if(tgt_type MATCHES "STATIC_LIBRARY|SHARED_LIBRARY|OBJECT_LIBRARY|MODULE_LIBRARY")
                set_target_properties(${tgt} PROPERTIES
                    MSVC_RUNTIME_LIBRARY
                    "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>\
$<$<CONFIG:Release>:MultiThreadedDLL>\
$<$<CONFIG:RelWithDebInfo>:MultiThreadedDLL>\
$<$<CONFIG:MinSizeRel>:MultiThreadedDLL>\
$<$<CONFIG:Distributed>:MultiThreaded>"
                )
            endif()
        endif()
    endforeach()

    foreach(internal_tgt alsoft-common common alcommon fmt)
        if(TARGET ${internal_tgt})
            get_target_property(aliased ${internal_tgt} ALIASED_TARGET)
            if(NOT aliased)
                set_target_properties(${internal_tgt} PROPERTIES
                    MSVC_RUNTIME_LIBRARY
                    "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>\
$<$<CONFIG:Release>:MultiThreadedDLL>\
$<$<CONFIG:RelWithDebInfo>:MultiThreadedDLL>\
$<$<CONFIG:MinSizeRel>:MultiThreadedDLL>\
$<$<CONFIG:Distributed>:MultiThreaded>"
                )
            endif()
        endif()
    endforeach()

    if(WIN32)
        target_link_libraries(OpenAL INTERFACE legacy_stdio_definitions.lib)
    endif()
else()

    add_subdirectory(${CMAKE_SOURCE_DIR}/External/openal)
endif()

    # Print OpenAL diagnostic
    get_target_property(_openal_type     OpenAL TYPE)
    get_target_property(_openal_includes OpenAL INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "OpenAL TYPE     = ${_openal_type}")
    message(STATUS "OpenAL INCLUDES = ${_openal_includes}")
endif() # NOT ANDROID

# ============================================
# Lua Library (static, all platforms)
# ============================================
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
set_target_properties(lua PROPERTIES LINKER_LANGUAGE C)
target_include_directories(lua PUBLIC "${CMAKE_SOURCE_DIR}/External/lua")

if(ANDROID)
    target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3 LUA_USE_POSIX)
elseif(UNIX)
    target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3 LUA_USE_POSIX LUA_USE_DLOPEN)
    if(NOT APPLE)
        target_link_libraries(lua PUBLIC dl)
    endif()
else()
    target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3 _CRT_SECURE_NO_WARNINGS)
endif()

set_msvc_runtime_library(lua)

# ============================================
# Box2D Library (static, all platforms)
# ============================================
file(GLOB_RECURSE BOX2D_SRC "${CMAKE_SOURCE_DIR}/External/box2d/src/*.cpp")
add_library(box2d STATIC ${BOX2D_SRC})
set_target_properties(box2d PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(box2d PUBLIC "${CMAKE_SOURCE_DIR}/External/box2d/include")
set_msvc_runtime_library(box2d)

# ============================================
# ImGui Library (static, platform-aware backends)
# ============================================
set(IMGUI_CORE_SRC
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_demo.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_tables.cpp"
)

if(ANDROID)
    set(IMGUI_BACKEND_SRC
        "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_android.cpp"
        "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp"
    )
else()
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
    target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/External/glfw/include)
endif()

if(ANDROID)
    target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_ES3)
else()
    target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
endif()

set_msvc_runtime_library(imgui)

# ============================================
# External / GLAD Library (platform-aware)
# ============================================
if(ANDROID)
    # Android: header-only interface target (no GLAD, no GLFW)
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
        ${CMAKE_SOURCE_DIR}/External/openal/include
        ${CMAKE_SOURCE_DIR}/External/audio
        ${CMAKE_SOURCE_DIR}/External/icons
    )
else()
    set(GLAD_SRC "${CMAKE_SOURCE_DIR}/External/glad/glad.cpp")
    add_library(External STATIC ${GLAD_SRC})
    set_msvc_runtime_library(External)

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
        ${CMAKE_SOURCE_DIR}/External/openal/include
        ${CMAKE_SOURCE_DIR}/External/audio
        ${CMAKE_SOURCE_DIR}/External/icons
    )

    target_compile_definitions(External PRIVATE
        GLFW_STATIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD
    )

    target_link_libraries(External PUBLIC glfw)

    if(WIN32)
        target_link_libraries(External PRIVATE
            opengl32.lib winmm ole32 user32 uuid
        )
    elseif(UNIX AND NOT APPLE)
        find_package(OpenGL REQUIRED)
        find_package(X11   REQUIRED)
        target_link_libraries(External PRIVATE
            OpenGL::GL
            ${X11_LIBRARIES} X11 Xrandr Xi Xxf86vm Xcursor Xinerama
            dl pthread
        )
        message(STATUS "X11 libraries: ${X11_LIBRARIES}")
    elseif(APPLE)
        find_package(OpenGL REQUIRED)
        target_link_libraries(External PRIVATE OpenGL::GL)
    endif()
endif()

# ============================================
# Application Library (static)
# ============================================
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
        "${CMAKE_SOURCE_DIR}/Application/src/Platform/Android/*.cpp")
else()
    file(GLOB APP_PLATFORM_SRC
        "${CMAKE_SOURCE_DIR}/Application/src/Platform/WindowsWindow/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenGL/*.cpp"
        "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenAL/*.cpp"
    )
endif()

set(APP_SRC ${APP_CORE_SRC} ${APP_PLATFORM_SRC})
if(NOT APP_PLATFORM_SRC)
    message(WARNING "No platform-specific sources found — falling back to glob all sources")
    file(GLOB_RECURSE APP_SRC "${CMAKE_SOURCE_DIR}/Application/src/*.cpp")
endif()

add_library(Application STATIC ${APP_SRC})
set_msvc_runtime_library(Application)

target_include_directories(Application PUBLIC
    ${CMAKE_SOURCE_DIR}/Application/include
    ${CMAKE_SOURCE_DIR}/External/box2d/include
    ${CMAKE_SOURCE_DIR}/External/lua
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
)

target_link_libraries(Application PUBLIC lua box2d imgui External)

target_compile_definitions(Application PRIVATE
    STB_IMAGE_IMPLEMENTATION
    DR_MP3_IMPLEMENTATION
    DR_WAV_IMPLEMENTATION
    $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>
)

# GTK3 for native file dialogs on Linux
if(UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_include_directories(Application PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(Application PRIVATE ${GTK3_LIBRARIES})
    target_compile_options(Application PRIVATE ${GTK3_CFLAGS_OTHER})
    message(STATUS "GTK3 found: ${GTK3_VERSION}")
endif()

if(ANDROID)
    message(STATUS "Configuring Android system libraries...")
    find_library(log-lib     log)
    find_library(android-lib android)
    find_library(EGL-lib     EGL)
    find_library(GLESv3-lib  GLESv3)
    find_library(OpenSLES-lib OpenSLES)
    target_link_libraries(Application PUBLIC
        ${log-lib} ${android-lib} ${EGL-lib} ${GLESv3-lib} ${OpenSLES-lib}
    )
    target_compile_definitions(Application PUBLIC IMGUI_IMPL_OPENGL_ES3)
else()
    target_link_libraries(Application PUBLIC OpenAL)
    if(WIN32)
        target_link_libraries(Application PRIVATE
            user32.lib gdi32.lib shell32.lib
            kernel32.lib winspool.lib ole32.lib oleaut32.lib
            uuid.lib comdlg32.lib advapi32.lib
            dxguid.lib winmm.lib dsound.lib oleacc.lib setupapi.lib
        )
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(Application PRIVATE pthread m)
    endif()
endif()

# ============================================
# Precompiled Headers
# ============================================
target_precompile_headers(Application PUBLIC
    "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
)

# ============================================
# Android Shared Library
# ============================================
if(ANDROID)
    message(STATUS "Creating Android shared library target...")

    add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    )
    target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue
    )

    add_library(aeroengine SHARED
        ${CMAKE_SOURCE_DIR}/Application/src/Platform/Android/android_main.cpp
    )
    target_include_directories(aeroengine PRIVATE
        ${CMAKE_SOURCE_DIR}/Application/include
    )
    target_link_libraries(aeroengine PRIVATE
        Application native_app_glue ${log-lib} ${android-lib}
    )

# ============================================
# Desktop Executables
# ============================================
else()
    message(STATUS "Creating desktop executable targets...")

    # --- AeroEditor ---
    file(GLOB_RECURSE AERO_SRC "${CMAKE_SOURCE_DIR}/AeroEditor/src/*.cpp")
    add_executable(AeroEditor ${AERO_SRC})
    set_msvc_runtime_library(AeroEditor)

    target_include_directories(AeroEditor PRIVATE
        ${CMAKE_SOURCE_DIR}/AeroEditor/include
    )
    target_link_libraries(AeroEditor PRIVATE Application)

    target_compile_definitions(AeroEditor PRIVATE
        $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>
    )

    if(WIN32)
        target_link_libraries(AeroEditor PRIVATE Shell32 Ole32)
        target_link_options(AeroEditor PRIVATE
            "$<$<CONFIG:Distributed>:/SUBSYSTEM:WINDOWS>"
            "$<$<CONFIG:Distributed>:/ENTRY:mainCRTStartup>"
        )
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(AeroEditor PRIVATE pthread dl)
    endif()

    target_precompile_headers(AeroEditor PRIVATE
        "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
    )

    # --- Sandbox ---
    file(GLOB_RECURSE SANDBOX_SRC "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp")
    add_executable(Sandbox ${SANDBOX_SRC})
    set_msvc_runtime_library(Sandbox)

    target_include_directories(Sandbox PRIVATE
        ${CMAKE_SOURCE_DIR}/Sandbox/include
    )
    target_link_libraries(Sandbox PRIVATE Application)

    target_compile_definitions(Sandbox PRIVATE
        $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>
    )

    if(WIN32)
        target_link_options(Sandbox PRIVATE
            "$<$<CONFIG:Distributed>:/SUBSYSTEM:WINDOWS>"
            "$<$<CONFIG:Distributed>:/ENTRY:mainCRTStartup>"
        )
    endif()

    target_precompile_headers(Sandbox PRIVATE
        "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
    )

    set_target_properties(AeroEditor Sandbox PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    # --- AssetPacker Tool ---
    add_executable(AssetPacker ${CMAKE_SOURCE_DIR}/tools/AssetPacker/main.cpp)
    target_include_directories(AssetPacker PRIVATE ${CMAKE_SOURCE_DIR}/Application/include)
    target_link_libraries(AssetPacker PRIVATE Application)
    target_precompile_headers(AssetPacker PRIVATE
        "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
    )
    set_msvc_runtime_library(AssetPacker)
    set_target_properties(AssetPacker PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
    )

    # --- Asset Packing Step ---
    file(GLOB_RECURSE ENGINE_ASSET_FILES "${CMAKE_SOURCE_DIR}/Application/assets/*")
    set(PAK_OUTPUT "${CMAKE_BINARY_DIR}/assets.pak")

    add_custom_command(
        OUTPUT  "${PAK_OUTPUT}"
        COMMAND AssetPacker "${CMAKE_SOURCE_DIR}/Application/assets" "${PAK_OUTPUT}"
        DEPENDS AssetPacker ${ENGINE_ASSET_FILES}
        COMMENT "Packing engine assets → assets.pak"
    )

    add_custom_target(PackEngineAssets ALL DEPENDS "${PAK_OUTPUT}")
    add_dependencies(AeroEditor PackEngineAssets)

    add_custom_command(TARGET AeroEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PAK_OUTPUT}"
            "$<TARGET_FILE_DIR:AeroEditor>/assets.pak"
        COMMENT "Copying assets.pak to AeroEditor output dir"
    )




    if(WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL "Distributed")
        get_target_property(_oal_type OpenAL TYPE)
        if(_oal_type STREQUAL "SHARED_LIBRARY")
            add_custom_command(TARGET AeroEditor POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:OpenAL>" "$<TARGET_FILE_DIR:AeroEditor>"
                COMMENT "Copying OpenAL DLL → AeroEditor"
                VERBATIM
            )
            add_custom_command(TARGET Sandbox POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:OpenAL>" "$<TARGET_FILE_DIR:Sandbox>"
                COMMENT "Copying OpenAL DLL → Sandbox"
                VERBATIM
            )
        endif()
    endif()


    include(InstallRequiredSystemLibraries)

    # --- Shared package metadata ---
    set(CPACK_PACKAGE_NAME                 "GameEngine")
    set(CPACK_PACKAGE_VENDOR               "YourStudio")
    set(CPACK_PACKAGE_VERSION              "1.0.0")
    set(CPACK_PACKAGE_VERSION_MAJOR        "1")
    set(CPACK_PACKAGE_VERSION_MINOR        "0")
    set(CPACK_PACKAGE_VERSION_PATCH        "0")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY  "Professional Game Engine")
    set(CPACK_PACKAGE_DESCRIPTION          "A professional cross-platform game engine with editor, scripting, and audio support.")
    set(CPACK_PACKAGE_CONTACT              "support@yourwebsite.com")
    set(CPACK_PACKAGE_HOMEPAGE_URL         "https://yourwebsite.com")

    # --- Shared install rules ---
    install(TARGETS AeroEditor Sandbox
        RUNTIME DESTINATION bin
        BUNDLE  DESTINATION .
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/Application/assets
        DESTINATION share/gameengine/assets
        USE_SOURCE_PERMISSIONS
    )
    foreach(doc README.md LICENSE.txt CHANGELOG.md)
        if(EXISTS "${CMAKE_SOURCE_DIR}/${doc}")
            install(FILES "${CMAKE_SOURCE_DIR}/${doc}" DESTINATION share/doc/gameengine)
        endif()
    endforeach()

    # ======================================================
    if(WIN32)
    # ======================================================
        # --- Icons ---
        if(EXISTS "${CMAKE_SOURCE_DIR}/icon.ico")
            set(CPACK_PACKAGE_ICON       "${CMAKE_SOURCE_DIR}/icon.ico")
            set(CPACK_NSIS_MUI_ICON      "${CMAKE_SOURCE_DIR}/icon.ico")
            set(CPACK_NSIS_MUI_UNIICON   "${CMAKE_SOURCE_DIR}/icon.ico")
        endif()

        set(CMAKE_INSTALL_PREFIX "C:/Program Files/GameEngine" CACHE PATH "Install path" FORCE)

        # --- NSIS generator ---
        set(CPACK_GENERATOR                    "NSIS")
        set(CPACK_NSIS_PACKAGE_NAME            "Game Engine")
        set(CPACK_NSIS_DISPLAY_NAME            "Game Engine v${CPACK_PACKAGE_VERSION}")
        set(CPACK_NSIS_INSTALLED_ICON_NAME     "bin\\\\AeroEditor.exe")
        set(CPACK_PACKAGE_INSTALL_DIRECTORY    "Game Engine")
        set(CPACK_NSIS_MODIFY_PATH             OFF)
        set(CPACK_NSIS_HELP_LINK               "https://github.com/yourname/game-engine")
        set(CPACK_NSIS_URL_INFO_ABOUT          "https://yourwebsite.com")
        set(CPACK_NSIS_CONTACT                 "support@yourwebsite.com")
        set(CPACK_NSIS_COMPRESSOR              "/SOLID lzma")
        set(CPACK_NSIS_COMPRESSOR_SET_THREADS  3)
        set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

        set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
            CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Aero Editor.lnk' '$INSTDIR\\\\bin\\\\AeroEditor.exe' '' '$INSTDIR\\\\bin\\\\AeroEditor.exe' 0
            CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Sandbox.lnk'     '$INSTDIR\\\\bin\\\\Sandbox.exe'    '' '$INSTDIR\\\\bin\\\\Sandbox.exe'    0
            CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Uninstall.lnk'   '$INSTDIR\\\\Uninstall.exe'

            CreateShortCut '$DESKTOP\\\\Aero Editor.lnk' '$INSTDIR\\\\bin\\\\AeroEditor.exe' '' '$INSTDIR\\\\bin\\\\AeroEditor.exe' 0
            CreateShortCut '$DESKTOP\\\\Sandbox.lnk'     '$INSTDIR\\\\bin\\\\Sandbox.exe'    '' '$INSTDIR\\\\bin\\\\Sandbox.exe'    0

            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'DisplayName'     'Game Engine'
            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'UninstallString' '$INSTDIR\\\\Uninstall.exe'
            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'InstallLocation' '$INSTDIR'
            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'DisplayVersion'  '${CPACK_PACKAGE_VERSION}'
            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'Publisher'       '${CPACK_PACKAGE_VENDOR}'
            WriteRegStr   HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'DisplayIcon'     '$INSTDIR\\\\bin\\\\AeroEditor.exe'
            WriteRegDWORD HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'NoModify' 1
            WriteRegDWORD HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine' 'NoRepair' 1

            CreateDirectory '$DOCUMENTS\\\\GameEngine Projects'
        ")

        set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
            Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Aero Editor.lnk'
            Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Sandbox.lnk'
            Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Uninstall.lnk'
            Delete '$DESKTOP\\\\Aero Editor.lnk'
            Delete '$DESKTOP\\\\Sandbox.lnk'

            MessageBox MB_YESNO 'Remove sample projects from Documents folder?' IDNO skip_samples
            RMDir /r '$DOCUMENTS\\\\GameEngine Projects'
            skip_samples:

            DeleteRegKey HKLM 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\GameEngine'
        ")

    # ======================================================
    elseif(UNIX AND NOT APPLE)
    # ======================================================
        # AFTER - gracefully skips if files don't exist
set(_desktop_files_ok TRUE)
foreach(_desktop_in aeroeditor.desktop.in sandbox.desktop.in)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/packaging/linux/${_desktop_in}")
        message(WARNING "Missing ${_desktop_in} — skipping .desktop installation")
        set(_desktop_files_ok FALSE)
    endif()
endforeach()

if(_desktop_files_ok)
    configure_file(
        "${CMAKE_SOURCE_DIR}/packaging/linux/aeroeditor.desktop.in"
        "${CMAKE_BINARY_DIR}/aeroeditor.desktop"
        @ONLY
    )
    configure_file(
        "${CMAKE_SOURCE_DIR}/packaging/linux/sandbox.desktop.in"
        "${CMAKE_BINARY_DIR}/sandbox.desktop"
        @ONLY
    )
    install(FILES
        "${CMAKE_BINARY_DIR}/aeroeditor.desktop"
        "${CMAKE_BINARY_DIR}/sandbox.desktop"
        DESTINATION share/applications
    )
endif()

        # Optional: install a PNG icon (create packaging/linux/gameengine.png)
        if(EXISTS "${CMAKE_SOURCE_DIR}/packaging/linux/gameengine.png")
            install(FILES "${CMAKE_SOURCE_DIR}/packaging/linux/gameengine.png"
                DESTINATION share/icons/hicolor/256x256/apps
                RENAME gameengine.png
            )
        endif()

        # Try to produce DEB + RPM; fall back to a plain tarball (TGZ) if
        # dpkg / rpmbuild are not available on the build machine.
        set(_cpack_generators "TGZ") # always available, used as fallback

        find_program(DPKG_FOUND dpkg)
        if(DPKG_FOUND)
            list(APPEND _cpack_generators "DEB")
            message(STATUS "dpkg found — will generate .deb package")
        endif()

        find_program(RPMBUILD_FOUND rpmbuild)
        if(RPMBUILD_FOUND)
            list(APPEND _cpack_generators "RPM")
            message(STATUS "rpmbuild found — will generate .rpm package")
        endif()

        set(CPACK_GENERATOR "${_cpack_generators}")
        message(STATUS "Linux CPack generators: ${_cpack_generators}")

        # --- DEB-specific ---
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER    "YourStudio <support@yourwebsite.com>")
        set(CPACK_DEBIAN_PACKAGE_SECTION       "games")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY      "optional")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE  "amd64")
        # Runtime deps: OpenAL is statically linked in Distributed, but GTK3,
        # X11, OpenGL and audio libs are expected to be present on the system.
        set(CPACK_DEBIAN_PACKAGE_DEPENDS
            "libgtk-3-0, libgl1, libx11-6, libxrandr2, libxi6, libasound2 | libpulse0"
        )
        set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
            "Professional Game Engine\n A cross-platform game engine with editor, Lua scripting,\n Box2D physics, OpenAL audio, and OpenGL rendering.")

        # --- RPM-specific ---
        set(CPACK_RPM_PACKAGE_LICENSE          "Proprietary")
        set(CPACK_RPM_PACKAGE_GROUP            "Amusements/Games")
        set(CPACK_RPM_PACKAGE_REQUIRES
            "gtk3, mesa-libGL, libX11, libXrandr, libXi, alsa-lib"
        )
        set(CPACK_RPM_PACKAGE_DESCRIPTION
            "A cross-platform game engine with editor, Lua scripting, Box2D physics, OpenAL audio, and OpenGL rendering."
        )

        # --- TGZ / shared ---
        set(CPACK_PACKAGE_FILE_NAME
            "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Linux-x86_64")
        set(CPACK_STRIP_FILES TRUE) # strip debug symbols from packaged binaries

    # ======================================================
    elseif(APPLE)
    # ======================================================
        set(CPACK_GENERATOR "DragNDrop") # produces a .dmg
        set(CPACK_DMG_VOLUME_NAME "Game Engine ${CPACK_PACKAGE_VERSION}")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-macOS")
        if(EXISTS "${CMAKE_SOURCE_DIR}/packaging/macos/DS_Store")
            set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/packaging/macos/DS_Store")
        endif()
        if(EXISTS "${CMAKE_SOURCE_DIR}/packaging/macos/background.png")
            set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/packaging/macos/background.png")
        endif()

    endif() # platform CPack

    include(CPack)

endif() # NOT ANDROID

# ============================================
# Configuration Summary
# ============================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS " Game Engine Configuration Summary")
message(STATUS "========================================")
message(STATUS " Platform    : ${PLATFORM_NAME}")
message(STATUS " C++ Standard: ${CMAKE_CXX_STANDARD}")
if(ANDROID)
    message(STATUS " Target      : Shared Library (libaeroengine.so)")
    message(STATUS " Graphics    : OpenGL ES 3.0")
    message(STATUS " Audio       : OpenSL ES")
else()
    message(STATUS " Targets     : AeroEditor, Sandbox, AssetPacker")
    message(STATUS " Graphics    : OpenGL 4.5 (GLAD)")
    if(WIN32)
        message(STATUS " Audio       : OpenAL (WASAPI / DirectSound / WinMM)")
    elseif(UNIX AND NOT APPLE)
        message(STATUS " Audio       : OpenAL (PulseAudio / ALSA)")
    elseif(APPLE)
        message(STATUS " Audio       : OpenAL (CoreAudio)")
    endif()
    message(STATUS " Window      : GLFW")
    message(STATUS " Build Types : Debug | Release | RelWithDebInfo | MinSizeRel | Distributed")
    if(WIN32)
        message(STATUS " Distributed : /MT static runtime, LTO (/GL/LTCG), NSIS installer")
    elseif(UNIX AND NOT APPLE)
        message(STATUS " Distributed : LTO (-flto), gc-sections, strip (-Wl,-s), DEB/RPM/TGZ")
    elseif(APPLE)
        message(STATUS " Distributed : LTO (-flto), dead_strip (-Wl,-dead_strip), .dmg")
    endif()
endif()
message(STATUS "========================================")
message(STATUS "")
