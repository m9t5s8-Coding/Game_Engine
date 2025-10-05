cmake_minimum_required(VERSION 3.20)

project(GameEngine)





# --- C++ standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB APP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(SRC
    ${APP_SRC}
)

add_library(System STATIC ${SRC})

target_include_directories(System
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


