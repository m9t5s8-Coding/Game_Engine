cmake_minimum_required(VERSION 3.20)


file(GLOB_RECURSE SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(AeroEditor ${SRC_FILES})

target_link_libraries(AeroEditor PRIVATE Application)

target_include_directories(AeroEditor PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)


add_custom_command(TARGET AeroEditor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/Application/assets"
  "$<TARGET_FILE_DIR:AeroEditor>/assets"
)


set_property(TARGET AeroEditor PROPERTY CXX_STANDARD 20)
set_property(TARGET AeroEditor PROPERTY CXX_STANDARD_REQUIRED ON)
