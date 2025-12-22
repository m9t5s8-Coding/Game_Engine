cmake_minimum_required(VERSION 3.20)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_compile_options(/utf-8)

project("Game_Engine" LANGUAGES C CXX)

# --- Force Distributed configuration for multi-config generators ---
# First, define ALL possible configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;Distributed" CACHE STRING 
    "Available build types: Debug, Release, RelWithDebInfo, MinSizeRel, Distributed" FORCE)

# If using multi-config generator (Visual Studio), set default to Distributed
if(CMAKE_CONFIGURATION_TYPES)
    # This ensures Distributed is recognized as a valid configuration
    set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE)
    
    # Map Distributed to use Release-like settings
    set(CMAKE_CXX_FLAGS_DISTRIBUTED "${CMAKE_CXX_FLAGS_RELEASE} /MT /GL /D DISTRIBUTED_BUILD=1")
    set(CMAKE_C_FLAGS_DISTRIBUTED "${CMAKE_C_FLAGS_RELEASE} /MT /GL /D DISTRIBUTED_BUILD=1")
    
    # Set output directories - CRITICAL FIX
    foreach(config IN ITEMS Debug Release RelWithDebInfo MinSizeRel Distributed)
        string(TOUPPER ${config} config_upper)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/${config}/bin")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/${config}/lib")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/${config}/lib")
        set(CMAKE_PDB_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/${config}/pdb")
    endforeach()
endif()

# Set default build type for single-config generators
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose build type" FORCE)
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel" "Distributed")
endif()

# --- C++ standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- MSVC-specific settings ---
if(MSVC)
    # Default compiler flags for each configuration
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_C_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /RTC1")
    
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
    
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG")
    
    set(CMAKE_CXX_FLAGS_MINSIZEREL "/MD /O1 /Ob1 /D NDEBUG")
    set(CMAKE_C_FLAGS_MINSIZEREL "/MD /O1 /Ob1 /D NDEBUG")
    
    # DISTRIBUTED: Static linking (/MT)
    set(CMAKE_CXX_FLAGS_DISTRIBUTED "/MT /O2 /Ob2 /D NDEBUG /D DISTRIBUTED_BUILD=1 /GL /Gy /arch:AVX2")
    set(CMAKE_C_FLAGS_DISTRIBUTED "/MT /O2 /Ob2 /D NDEBUG /D DISTRIBUTED_BUILD=1 /GL /Gy")
    
    # Linker flags for Distributed
    set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTED "/INCREMENTAL:NO /LTCG /OPT:REF /OPT:ICF /DEBUG:FULL")
    
    message(STATUS "MSVC: Distributed configuration will use /MT (static runtime)")
endif()

# --- GLFW configuration ---
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw)

if (MSVC)
    set_target_properties(glfw PROPERTIES
        MSVC_RUNTIME_LIBRARY "$<IF:$<CONFIG:Distributed>,MultiThreaded,MultiThreaded$<$<CONFIG:Debug>:Debug>DLL>"
    )
endif()

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

file(GLOB_RECURSE BOX2D_SRC
  "${CMAKE_SOURCE_DIR}/External/box2d/src/*.cpp"
)

set_source_files_properties(${LUA_SRC} PROPERTIES LANGUAGE C)
set_source_files_properties(${BOX2D_SRC} PROPERTIES LANGUAGE CXX)
add_library(lua STATIC ${LUA_SRC})
add_library(box2d STATIC ${BOX2D_SRC})
set_target_properties(lua PROPERTIES LINKER_LANGUAGE C)
set_target_properties(box2d PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(lua PUBLIC
    "${CMAKE_SOURCE_DIR}/External/lua"
)
target_include_directories(box2d PUBLIC
    "${CMAKE_SOURCE_DIR}/External/box2d/include"
)

target_compile_definitions(lua PRIVATE
    LUA_COMPAT_5_3
    _CRT_SECURE_NO_WARNINGS
)

# Force static runtime for lua and box2d in Distributed builds
if(MSVC)
    set_target_properties(lua box2d PROPERTIES
        MSVC_RUNTIME_LIBRARY "$<IF:$<CONFIG:Distributed>,MultiThreaded,MultiThreaded$<$<CONFIG:Debug>:Debug>DLL>"
    )
endif()

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

# Force static runtime for External in Distributed builds
if(MSVC)
    set_target_properties(External PROPERTIES
        MSVC_RUNTIME_LIBRARY "$<IF:$<CONFIG:Distributed>,MultiThreaded,MultiThreaded$<$<CONFIG:Debug>:Debug>DLL>"
    )
endif()

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

# Add DISTRIBUTED_BUILD definition to External library
target_compile_definitions(External PRIVATE
    GLFW_STATIC
    STB_IMAGE_IMPLEMENTATION
    SOL_LUA_VERSION=504
    $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>
)

# --- Application Library ---
file(GLOB_RECURSE APP_SRC 
    "${CMAKE_SOURCE_DIR}/Application/src/*.cpp"
)

add_library(Application STATIC ${APP_SRC})

# Force static runtime for Application in Distributed builds
if(MSVC)
    set_target_properties(Application PROPERTIES
        MSVC_RUNTIME_LIBRARY "$<IF:$<CONFIG:Distributed>,MultiThreaded,MultiThreaded$<$<CONFIG:Debug>:Debug>DLL>"
    )
endif()

# Include directories for Application
target_include_directories(Application PUBLIC
    ${CMAKE_SOURCE_DIR}/Application/include
)

# Link libraries for Application
target_link_libraries(Application PUBLIC 
    lua
    glfw
    box2d
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

# Add DISTRIBUTED_BUILD definition to Application
target_compile_definitions(Application PRIVATE 
    GLFW_STATIC
    STB_IMAGE_IMPLEMENTATION
    $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>
)

# --- AeroEditor Executable ---
file(GLOB_RECURSE AERO_SRC
    "${CMAKE_SOURCE_DIR}/AeroEditor/src/*.cpp"
)

add_executable(AeroEditor ${AERO_SRC})

# Force static runtime for AeroEditor in Distributed builds
if(MSVC)
    set_target_properties(AeroEditor PROPERTIES
        MSVC_RUNTIME_LIBRARY "$<IF:$<CONFIG:Distributed>,MultiThreaded,MultiThreaded$<$<CONFIG:Debug>:Debug>DLL>"
    )
endif()

# Link Application (which brings everything else)
target_link_libraries(AeroEditor PRIVATE Application)

# Include directories for AeroEditor
target_include_directories(AeroEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/AeroEditor/include
)

# Add DISTRIBUTED_BUILD definition to AeroEditor
target_compile_definitions(AeroEditor PRIVATE
    $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>
)

# Copy assets
add_custom_command(TARGET AeroEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
    "$<TARGET_FILE_DIR:AeroEditor>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Application/assets"
    "$<TARGET_FILE_DIR:AeroEditor>/assets"
    VERBATIM
)

# --- Sandbox Executable ---
file(GLOB_RECURSE SANDBOX_SRC
    "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp"
)

add_executable(Sandbox ${SANDBOX_SRC})

# Force static runtime for Sandbox in Distributed builds
if(MSVC)
    set_target_properties(Sandbox PROPERTIES
        MSVC_RUNTIME_LIBRARY "$<IF:$<CONFIG:Distributed>,MultiThreaded,MultiThreaded$<$<CONFIG:Debug>:Debug>DLL>"
    )
endif()



# Link Application (which brings everything else)
target_link_libraries(Sandbox PRIVATE Application)

# Include directories for Sandbox
target_include_directories(Sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/Sandbox/include
)

# Add DISTRIBUTED_BUILD definition to Sandbox
target_compile_definitions(Sandbox PRIVATE
    $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>
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
message(STATUS "Available configurations: ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
if(MSVC)
    message(STATUS "Compiler: MSVC")
    message(STATUS "Distributed build uses: /MT (static runtime)")
endif()
message(STATUS "External library links to: lua")
message(STATUS "Application library links to: glfw, External")
message(STATUS "AeroEditor links to: Application")
message(STATUS "Sandbox links to: Application")