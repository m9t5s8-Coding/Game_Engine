

file(GLOB_RECURSE SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(Engine ${SRC_FILES})

target_link_libraries(Engine PRIVATE Application)

target_include_directories(Engine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_property(TARGET Engine PROPERTY CXX_STANDARD 20)
set_property(TARGET Engine PROPERTY CXX_STANDARD_REQUIRED ON)
