# -----------------------------------------------------------------------------
# AeroEditor (desktop only)
# -----------------------------------------------------------------------------

if(NOT AERO_DESKTOP OR NOT BUILD_EDITOR)
  return()
endif()

file(GLOB_RECURSE AERO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(AeroEditor ${AERO_SRC})
set_msvc_runtime_library(AeroEditor)
target_include_directories(AeroEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(AeroEditor PRIVATE ApplicationEditor)
target_compile_definitions(
  AeroEditor PRIVATE AERO_EDITOR $<$<CONFIG:Distributed>:DISTRIBUTED_BUILD=1>)
if(WIN32)
  target_link_libraries(AeroEditor PRIVATE Shell32 Ole32)
  target_link_options(
    AeroEditor PRIVATE "$<$<CONFIG:Distributed>:/SUBSYSTEM:WINDOWS>"
    "$<$<CONFIG:Distributed>:/ENTRY:mainCRTStartup>")
elseif(UNIX AND NOT APPLE)
  target_link_libraries(AeroEditor PRIVATE pthread dl)
endif()
target_precompile_headers(AeroEditor PRIVATE
                          "${CMAKE_SOURCE_DIR}/Application/include/Apch.hpp")

# The editor needs the packed engine assets in its output directory.
if(TARGET PackEngineAssets)
  add_dependencies(AeroEditor PackEngineAssets)
  add_custom_command(
    TARGET AeroEditor
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AERO_ASSETS_PAK}"
            "$<TARGET_FILE_DIR:AeroEditor>/assets.pak"
    COMMENT "Copying assets.pak to the AeroEditor output directory")
endif()

aero_copy_openal_dll(AeroEditor)
