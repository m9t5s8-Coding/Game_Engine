cmake_minimum_required(VERSION 3.20)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("Game_Engine" LANGUAGES C CXX)

if(MSVC)
  add_compile_options(/utf-8)
endif()

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw)






set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)

set(ALSOFT_REQUIRE_OPENSL OFF CACHE BOOL "" FORCE)
set(ALSOFT_REQUIRE_WINMM ON CACHE BOOL "" FORCE)
set(ALSOFT_REQUIRE_DSOUND ON CACHE BOOL "" FORCE)
set(ALSOFT_REQUIRE_WASAPI ON CACHE BOOL "" FORCE)

set(ALSOFT_BACKEND_WINMM ON CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_DSOUND ON CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_WASAPI ON CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_PORTAUDIO OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_PULSEAUDIO OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_ALSA OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_COREAUDIO OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_OSS OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_SNDIO OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_SOLARIS OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_SOUNDIO OFF CACHE BOOL "" FORCE)
set(ALSOFT_BACKEND_WAVE OFF CACHE BOOL "" FORCE)

set(LIBTYPE "STATIC" CACHE STRING "Library type" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/External/openal)


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

file(GLOB_RECURSE BOX2D_SRC
  "${CMAKE_SOURCE_DIR}/External/box2d/src/*.cpp"
)

add_library(lua STATIC ${LUA_SRC})
add_library(box2d STATIC ${BOX2D_SRC})

target_include_directories(lua PUBLIC "${CMAKE_SOURCE_DIR}/External/lua")
target_include_directories(box2d PUBLIC "${CMAKE_SOURCE_DIR}/External/box2d/include")

target_compile_definitions(lua PRIVATE
    LUA_COMPAT_5_3
    _CRT_SECURE_NO_WARNINGS
)

set(GLAD_SRC "${CMAKE_SOURCE_DIR}/External/glad/glad.cpp")

set(IMGUI_SRC
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_demo.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_tables.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(External STATIC ${GLAD_SRC} ${IMGUI_SRC})

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

if(WIN32)
    target_link_libraries(External PRIVATE opengl32.lib)
endif()

if(WIN32)
    target_link_libraries(External PRIVATE
        winmm
        ole32
        user32
        uuid
    )
endif()

file(GLOB_RECURSE APP_SRC "${CMAKE_SOURCE_DIR}/Application/src/*.cpp")
add_library(Application STATIC ${APP_SRC})

target_include_directories(Application PUBLIC
    ${CMAKE_SOURCE_DIR}/Application/include
    ${CMAKE_SOURCE_DIR}/External/box2d/include 
    ${CMAKE_SOURCE_DIR}/External/lua         
)

target_link_libraries(Application PUBLIC
    lua
    box2d
    External
    OpenAL
)

target_compile_definitions(Application PRIVATE 
    STB_IMAGE_IMPLEMENTATION
    DR_MP3_IMPLEMENTATION
    DR_WAV_IMPLEMENTATION
)

if(WIN32)
    target_link_libraries(Application PRIVATE
        user32.lib
        gdi32.lib
        shell32.lib
    )
endif()

file(GLOB_RECURSE AERO_SRC "${CMAKE_SOURCE_DIR}/AeroEditor/src/*.cpp")
add_executable(AeroEditor ${AERO_SRC})

target_include_directories(AeroEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/AeroEditor/include
)

target_link_libraries(AeroEditor PRIVATE Application)

add_custom_command(TARGET AeroEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Application/assets"
    "$<TARGET_FILE_DIR:AeroEditor>/assets"
)

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

get_target_property(_openal_type OpenAL TYPE)
get_target_property(_openal_includes OpenAL INTERFACE_INCLUDE_DIRECTORIES)

message(STATUS "OpenAL TYPE = ${_openal_type}")
message(STATUS "OpenAL INCLUDES = ${_openal_includes}")
