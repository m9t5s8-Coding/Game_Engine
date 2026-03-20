cmake_minimum_required(VERSION 3.20)

project("Game_Engine" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Default build type ────────────────────────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# ── Platform detection ────────────────────────────────────────────────────────
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
    add_definitions(-DPLATFORM_MACOS)
elseif(UNIX)
    add_definitions(-DPLATFORM_LINUX)
endif()

if(MSVC)
    add_compile_options(/utf-8 /bigobj)
endif()

# ── Output directories ────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ── MSVC runtime helper ───────────────────────────────────────────────────────
function(set_msvc_runtime target)
    if(MSVC)
        set_target_properties(${target} PROPERTIES
            MSVC_RUNTIME_LIBRARY
            "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<NOT:$<CONFIG:Debug>>:MultiThreadedDLL>"
        )
    endif()
endfunction()

# ── GLFW ──────────────────────────────────────────────────────────────────────
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw)
set_msvc_runtime(glfw)

# ── OpenAL-Soft ───────────────────────────────────────────────────────────────
set(ALSOFT_UTILS                  OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES               OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS                  OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL                OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG         OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA      OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES       OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS          OFF CACHE BOOL "" FORCE)
set(LIBTYPE "STATIC" CACHE STRING "" FORCE)

if(WIN32)
    set(ALSOFT_REQUIRE_WINMM  ON  CACHE BOOL "" FORCE)
    set(ALSOFT_REQUIRE_DSOUND ON  CACHE BOOL "" FORCE)
    set(ALSOFT_REQUIRE_WASAPI ON  CACHE BOOL "" FORCE)
elseif(UNIX AND NOT APPLE)
    set(ALSOFT_BACKEND_ALSA        ON  CACHE BOOL "" FORCE)
    set(ALSOFT_BACKEND_PULSEAUDIO  ON  CACHE BOOL "" FORCE)
elseif(APPLE)
    set(ALSOFT_BACKEND_COREAUDIO   ON  CACHE BOOL "" FORCE)
endif()

if(MSVC)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<NOT:$<CONFIG:Debug>>:MultiThreadedDLL>"
    )
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/External/openal)

if(MSVC)
    get_directory_property(_oal_targets
        DIRECTORY ${CMAKE_SOURCE_DIR}/External/openal BUILDSYSTEM_TARGETS)
    foreach(tgt ${_oal_targets})
        if(TARGET ${tgt})
            get_target_property(_type ${tgt} TYPE)
            if(_type MATCHES "STATIC_LIBRARY|SHARED_LIBRARY|OBJECT_LIBRARY|MODULE_LIBRARY")
                set_target_properties(${tgt} PROPERTIES
                    MSVC_RUNTIME_LIBRARY
                    "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<NOT:$<CONFIG:Debug>>:MultiThreadedDLL>"
                )
            endif()
        endif()
    endforeach()
    if(WIN32)
        target_link_libraries(OpenAL INTERFACE legacy_stdio_definitions.lib)
    endif()
endif()

# ── Lua ───────────────────────────────────────────────────────────────────────
file(GLOB LUA_SRC "${CMAKE_SOURCE_DIR}/External/lua/*.c")
list(FILTER LUA_SRC EXCLUDE REGEX "lua\\.c$|luac\\.c$")  # exclude standalone interpreter/compiler
add_library(lua STATIC ${LUA_SRC})
set_target_properties(lua PROPERTIES LINKER_LANGUAGE C)
target_include_directories(lua PUBLIC "${CMAKE_SOURCE_DIR}/External/lua")
target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3 _CRT_SECURE_NO_WARNINGS)
if(UNIX)
    target_compile_definitions(lua PRIVATE LUA_USE_POSIX LUA_USE_DLOPEN)
    if(NOT APPLE)
        target_link_libraries(lua PUBLIC dl)
    endif()
endif()
set_msvc_runtime(lua)

# ── Box2D ─────────────────────────────────────────────────────────────────────
file(GLOB_RECURSE BOX2D_SRC "${CMAKE_SOURCE_DIR}/External/box2d/src/*.cpp")
add_library(box2d STATIC ${BOX2D_SRC})
target_include_directories(box2d PUBLIC "${CMAKE_SOURCE_DIR}/External/box2d/include")
set_msvc_runtime(box2d)

# ── ImGui ─────────────────────────────────────────────────────────────────────
add_library(imgui STATIC
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_demo.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_tables.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
    ${CMAKE_SOURCE_DIR}/External/glfw/include
)
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
set_msvc_runtime(imgui)

# ── External (GLAD + headers) ─────────────────────────────────────────────────
add_library(External STATIC "${CMAKE_SOURCE_DIR}/External/glad/glad.cpp")
set_msvc_runtime(External)

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

target_compile_definitions(External PRIVATE GLFW_STATIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(External PUBLIC glfw)

if(WIN32)
    target_link_libraries(External PRIVATE opengl32.lib winmm ole32 user32 uuid)
elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    find_package(X11   REQUIRED)
    target_link_libraries(External PRIVATE
        OpenGL::GL ${X11_LIBRARIES} X11 Xrandr Xi Xxf86vm Xcursor Xinerama dl pthread)
elseif(APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(External PRIVATE OpenGL::GL)
endif()

# ── Application library ───────────────────────────────────────────────────────
file(GLOB_RECURSE APP_CORE_SRC
    "${CMAKE_SOURCE_DIR}/Application/src/Audio/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Core/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/GameObjects/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/ImGui/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Layers/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Networking/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Project/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Renderer/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Scene/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Scripting/*.cpp"
)

file(GLOB APP_PLATFORM_SRC
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/WindowsWindow/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenGL/*.cpp"
    "${CMAKE_SOURCE_DIR}/Application/src/Platform/OpenAL/*.cpp"
)

add_library(Application STATIC ${APP_CORE_SRC} ${APP_PLATFORM_SRC})
set_msvc_runtime(Application)

target_include_directories(Application PUBLIC
    ${CMAKE_SOURCE_DIR}/Application/include
    ${CMAKE_SOURCE_DIR}/External/box2d/include
    ${CMAKE_SOURCE_DIR}/External/lua
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
)

target_compile_definitions(Application PRIVATE
    STB_IMAGE_IMPLEMENTATION
    DR_MP3_IMPLEMENTATION
    DR_WAV_IMPLEMENTATION
)

target_link_libraries(Application PUBLIC lua box2d imgui External OpenAL)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_include_directories(Application PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(Application PRIVATE ${GTK3_LIBRARIES})
    target_compile_options(Application PRIVATE ${GTK3_CFLAGS_OTHER})
    target_link_libraries(Application PRIVATE pthread m)
endif()

if(WIN32)
    target_link_libraries(Application PRIVATE
        user32.lib gdi32.lib shell32.lib kernel32.lib
        winspool.lib ole32.lib oleaut32.lib uuid.lib
        comdlg32.lib advapi32.lib dxguid.lib
        winmm.lib dsound.lib oleacc.lib setupapi.lib
    )
endif()

target_precompile_headers(Application PUBLIC
    "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
)

# ── AeroEditor ────────────────────────────────────────────────────────────────
file(GLOB_RECURSE AERO_SRC "${CMAKE_SOURCE_DIR}/AeroEditor/src/*.cpp")
add_executable(AeroEditor ${AERO_SRC})
set_msvc_runtime(AeroEditor)
target_include_directories(AeroEditor PRIVATE ${CMAKE_SOURCE_DIR}/AeroEditor/include)
target_link_libraries(AeroEditor PRIVATE Application)
target_precompile_headers(AeroEditor PRIVATE
    "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
)
if(WIN32)
    target_link_libraries(AeroEditor PRIVATE Shell32 Ole32)
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(AeroEditor PRIVATE pthread dl)
endif()

# ── Sandbox ───────────────────────────────────────────────────────────────────
file(GLOB_RECURSE SANDBOX_SRC "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp")
add_executable(Sandbox ${SANDBOX_SRC})
set_msvc_runtime(Sandbox)
target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}/Sandbox/include)
target_link_libraries(Sandbox PRIVATE Application)
target_precompile_headers(Sandbox PRIVATE
    "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
)


# ServerSandbox
file(GLOB_RECURSE SERVER_SANDBOX_SRC "${CMAKE_SOURCE_DIR}/ServerSandbox/src/*.cpp")
add_executable(ServerSandbox ${SERVER_SANDBOX_SRC})
set_msvc_runtime(ServerSandbox)
target_include_directories(ServerSandbox PRIVATE ${CMAKE_SOURCE_DIR}/ServerSandbox/include)



add_library(ApplicationServer STATIC ${APP_CORE_SRC} ${APP_PLATFORM_SRC})
set_msvc_runtime(ApplicationServer)

target_include_directories(ApplicationServer PUBLIC
    ${CMAKE_SOURCE_DIR}/Application/include
    ${CMAKE_SOURCE_DIR}/External/box2d/include
    ${CMAKE_SOURCE_DIR}/External/lua
    ${CMAKE_SOURCE_DIR}/External/imgui
    ${CMAKE_SOURCE_DIR}/External/imgui/backends
)

target_compile_definitions(ApplicationServer PRIVATE
    STB_IMAGE_IMPLEMENTATION
    DR_MP3_IMPLEMENTATION
    DR_WAV_IMPLEMENTATION
    AERO_SERVER
)

target_link_libraries(ApplicationServer PUBLIC lua box2d imgui External OpenAL)

# Add the GTK3 linkage for Linux
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_include_directories(ApplicationServer PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(ApplicationServer PRIVATE ${GTK3_LIBRARIES})
    target_compile_options(ApplicationServer PRIVATE ${GTK3_CFLAGS_OTHER})
    target_link_libraries(ApplicationServer PRIVATE pthread m)
endif()

# Add Windows libs
if(WIN32)
    target_link_libraries(ApplicationServer PRIVATE
        user32.lib gdi32.lib shell32.lib kernel32.lib
        winspool.lib ole32.lib oleaut32.lib uuid.lib
        comdlg32.lib advapi32.lib dxguid.lib
        winmm.lib dsound.lib oleacc.lib setupapi.lib
    )
endif()

# PCH
target_precompile_headers(ApplicationServer PUBLIC
    "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
)


target_link_libraries(ServerSandbox PRIVATE ApplicationServer)
target_compile_definitions(ServerSandbox PRIVATE AERO_SERVER)

if(WIN32)
  target_link_libraries(ServerSandbox PRIVATE Shell32 Ole32)
endif()
if(UNIX AND NOT APPLE)
  target_link_libraries(ServerSandbox PRIVATE pthread dl)
endif()



# ── AssetPacker ───────────────────────────────────────────────────────────────
# add_executable(AssetPacker ${CMAKE_SOURCE_DIR}/tools/AssetPacker/main.cpp)
# target_include_directories(AssetPacker PRIVATE ${CMAKE_SOURCE_DIR}/Application/include)
# target_link_libraries(AssetPacker PRIVATE Application)
# target_precompile_headers(AssetPacker PRIVATE
#     "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
# )
# set_msvc_runtime(AssetPacker)
# set_target_properties(AssetPacker PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
#     EXCLUDE_FROM_ALL TRUE
# )

# ── Asset packing step ────────────────────────────────────────────────────────
# file(GLOB_RECURSE ENGINE_ASSET_FILES "${CMAKE_SOURCE_DIR}/Application/assets/*")
# set(PAK_OUTPUT "${CMAKE_BINARY_DIR}/assets.pak")
#
# add_custom_command(
#     OUTPUT  "${PAK_OUTPUT}"
#     COMMAND AssetPacker "${CMAKE_SOURCE_DIR}/Application/assets" "${PAK_OUTPUT}"
#     DEPENDS AssetPacker ${ENGINE_ASSET_FILES}
#     COMMENT "Packing engine assets → assets.pak"
# )
# add_custom_target(PackEngineAssets ALL DEPENDS "${PAK_OUTPUT}")
# add_dependencies(AeroEditor PackEngineAssets)
#
# add_custom_command(TARGET AeroEditor POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "${PAK_OUTPUT}" "$<TARGET_FILE_DIR:AeroEditor>/assets.pak"
#     COMMENT "Copying assets.pak → AeroEditor output dir"
# )

# ── Copy OpenAL DLL on Windows (if shared) ────────────────────────────────────
if(WIN32)
    get_target_property(_oal_type OpenAL TYPE)
    if(_oal_type STREQUAL "SHARED_LIBRARY")
      foreach(tgt AeroEditor Sandbox ServerSandbox)
            add_custom_command(TARGET ${tgt} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:OpenAL>" "$<TARGET_FILE_DIR:${tgt}>"
                COMMENT "Copying OpenAL DLL → ${tgt}" VERBATIM
            )
        endforeach()
    endif()
endif()

# ── Summary ───────────────────────────────────────────────────────────────────
message(STATUS "")
message(STATUS "=== Game Engine (Dev Build) ===")
message(STATUS " Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS " Targets    : AeroEditor  Sandbox  ServerSandbox  AssetPacker")
message(STATUS " C++        : ${CMAKE_CXX_STANDARD}")
message(STATUS "==============================")
message(STATUS "")
