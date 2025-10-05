cmake_minimum_required(VERSION 3.20)

project(GameEngine)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE APP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(SRC
    ${APP_SRC}
)

add_library(Graphics STATIC ${SRC})

target_link_libraries(Graphics PUBLIC
  System
)

target_include_directories(Graphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


