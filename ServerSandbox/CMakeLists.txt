# -----------------------------------------------------------------------------
# ServerSandbox (headless server, desktop only)
# -----------------------------------------------------------------------------

if(NOT AERO_DESKTOP OR NOT BUILD_SERVER)
  return()
endif()

file(GLOB_RECURSE SERVER_SANDBOX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(ServerSandbox ${SERVER_SANDBOX_SRC})
set_msvc_runtime_library(ServerSandbox)
target_include_directories(ServerSandbox
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ServerSandbox PRIVATE ApplicationServer)
target_compile_definitions(
  ServerSandbox
  PRIVATE AERO_SERVER $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>)
if(WIN32)
  target_link_libraries(ServerSandbox PRIVATE Shell32 Ole32)
  target_link_options(
    ServerSandbox PRIVATE "$<$<CONFIG:Distributed>:/SUBSYSTEM:WINDOWS>"
    "$<$<CONFIG:Distributed>:/ENTRY:mainCRTStartup>")
elseif(UNIX AND NOT APPLE)
  target_link_libraries(ServerSandbox PRIVATE pthread dl)
endif()
target_precompile_headers(ServerSandbox PRIVATE
                          "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp")

aero_copy_openal_dll(ServerSandbox)
