cmake_minimum_required(VERSION 3.20)


file(GLOB_RECURSE SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(Sandbox ${SRC_FILES})

target_link_libraries(Sandbox PRIVATE Application)

target_include_directories(Sandbox PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_property(TARGET Sandbox PROPERTY CXX_STANDARD 20)
set_property(TARGET Sandbox PROPERTY CXX_STANDARD_REQUIRED ON)
