cmake_minimum_required(VERSION 3.20)


file(GLOB_RECURSE SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(Sandbox ${SRC_FILES})

target_link_libraries(Sandbox PRIVATE Application)

target_include_directories(Sandbox PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_custom_command(TARGET Sandbox POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/Application/shaders"
  "$<TARGET_FILE_DIR:Sandbox>/shaders"
)

add_custom_command(TARGET Sandbox POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/Application/assets"
  "$<TARGET_FILE_DIR:Sandbox>/assets"
)


set_property(TARGET Sandbox PROPERTY CXX_STANDARD 20)
set_property(TARGET Sandbox PROPERTY CXX_STANDARD_REQUIRED ON)
