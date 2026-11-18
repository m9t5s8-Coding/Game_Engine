# -----------------------------------------------------------------------------
# AssetPacker CLI + engine-asset packing target
# -----------------------------------------------------------------------------

if(NOT AERO_DESKTOP)
  return()
endif()

add_executable(AssetPacker main.cpp)
set_msvc_runtime_library(AssetPacker)
target_include_directories(AssetPacker
                           PRIVATE ${CMAKE_SOURCE_DIR}/Application/include)
set_target_properties(
  AssetPacker
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF
             EXCLUDE_FROM_ALL TRUE)

if(AERO_ENGINE_ASSETS_ENABLED)
  file(GLOB_RECURSE ENGINE_ASSET_FILES
       "${CMAKE_SOURCE_DIR}/Application/assets/*")

  add_custom_command(
    OUTPUT "${AERO_ASSETS_PAK}"
    COMMAND AssetPacker "${CMAKE_SOURCE_DIR}/Application/assets"
            "${AERO_ASSETS_PAK}"
    DEPENDS AssetPacker ${ENGINE_ASSET_FILES}
    COMMENT "Packing engine assets -> assets.pak")

  add_custom_target(PackEngineAssets ALL DEPENDS "${AERO_ASSETS_PAK}")
endif()
