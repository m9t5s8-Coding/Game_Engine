cmake_minimum_required(VERSION 3.20)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_compile_options(/utf-8)

project("Game_Engine" LANGUAGES C CXX)

# --- C++ standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- GLFW configuration ---
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw)

# --- Lua Library (Built as C) ---
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

set_source_files_properties(${LUA_SRC} PROPERTIES LANGUAGE C)
add_library(lua STATIC ${LUA_SRC})
set_target_properties(lua PROPERTIES LINKER_LANGUAGE C)

target_include_directories(lua PUBLIC
    "${CMAKE_SOURCE_DIR}/External/lua"
)

target_compile_definitions(lua PRIVATE
    LUA_USE_WINDOWS
    LUA_COMPAT_5_3
    _CRT_SECURE_NO_WARNINGS
)



# --- External Library (GLAD + ImGui) ---
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

add_library(External STATIC 
    ${GLAD_SRC}
    ${IMGUI_SRC}
)



# Include directories for External
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
)

# Compile definitions for External
target_compile_definitions(External PRIVATE
    GLFW_STATIC
    STB_IMAGE_IMPLEMENTATION
    SOL_LUA_VERSION=504
)

# --- Application Library ---
file(GLOB_RECURSE APP_SRC 
    "${CMAKE_SOURCE_DIR}/Application/src/*.cpp"
)

add_library(Application STATIC ${APP_SRC})

# Include directories for Application
target_include_directories(Application PUBLIC
    ${CMAKE_SOURCE_DIR}/Application/include
)

# Link libraries for Application
target_link_libraries(Application PUBLIC 
    lua
    glfw
    External
   
)

# System libraries (Windows)
if(WIN32)
    target_link_libraries(Application PRIVATE
        opengl32.lib
        user32.lib
        gdi32.lib
        shell32.lib
        kernel32.lib
        winspool.lib
        ole32.lib
        oleaut32.lib
        uuid.lib
        comdlg32.lib
        advapi32.lib
    )
endif()

# Compile definitions for Application
target_compile_definitions(Application PRIVATE 
    GLFW_STATIC
    STB_IMAGE_IMPLEMENTATION
)

# --- AeroEditor Executable ---
file(GLOB_RECURSE AERO_SRC
    "${CMAKE_SOURCE_DIR}/AeroEditor/src/*.cpp"
)

add_executable(AeroEditor ${AERO_SRC})

# Link Application (which brings everything else)
target_link_libraries(AeroEditor PRIVATE Application)

# Include directories for AeroEditor
target_include_directories(AeroEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/AeroEditor/include
)

# Copy assets
add_custom_command(TARGET AeroEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Application/assets"
    "$<TARGET_FILE_DIR:AeroEditor>/assets"
)

# --- Sandbox Executable ---
file(GLOB_RECURSE SANDBOX_SRC
    "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp"
)

add_executable(Sandbox ${SANDBOX_SRC})

# Link Application (which brings everything else)
target_link_libraries(Sandbox PRIVATE Application)

# Include directories for Sandbox
target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/Sandbox/include
)

# Set C++ standard for executables
set_target_properties(AeroEditor Sandbox PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_precompile_headers(Application PUBLIC
    "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
)

# Optionally add to executables too
target_precompile_headers(AeroEditor PRIVATE
    "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
)

target_precompile_headers(Sandbox PRIVATE
    "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp"
)

# --- Debug Output ---
message(STATUS "=== BUILD CONFIGURATION ===")
message(STATUS "External library links to: lua")
message(STATUS "Application library links to: glfw, External")
message(STATUS "AeroEditor links to: Application")
message(STATUS "Sandbox links to: Application")