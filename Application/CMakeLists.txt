cmake_minimum_required(VERSION 3.16)
project(GameEngine)

# --- C++ standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Application sources ---
file(GLOB APP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# --- GLAD ---
set(GLAD_SRC "${CMAKE_SOURCE_DIR}/External/glad/glad.c")

# --- ImGui ---
set(IMGUI_SRC
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_widgets.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/imgui_tables.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/External/imgui/backends/imgui_impl_opengl3.cpp"
)

# --- Combine all sources ---
set(SRC
    ${APP_SRC}
    ${GLAD_SRC}
    ${IMGUI_SRC}
)


message(STATUS "APP_SRC = ${APP_SRC}")
message(STATUS "GLAD_SRC = ${GLAD_SRC}")
message(STATUS "IMGUI_SRC = ${IMGUI_SRC}")
message(STATUS "SRC = ${SRC}")


# --- Add Application library ---
add_library(Application STATIC ${SRC})

# --- Include directories ---
target_include_directories(Application
    PRIVATE
        ${CMAKE_SOURCE_DIR}/External/glfw/include
        ${CMAKE_SOURCE_DIR}/External/glad/include
        ${CMAKE_SOURCE_DIR}/External/glm
        ${CMAKE_SOURCE_DIR}/External/spdlog/include
        ${CMAKE_SOURCE_DIR}/External/imgui
        ${CMAKE_SOURCE_DIR}/External/imgui/backends
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)



# Link GLFW to Application
target_link_libraries(Application PRIVATE glfw)

# --- System libraries (Windows) ---
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

# --- Compile definitions ---
target_compile_definitions(Application PRIVATE GLFW_STATIC)
# target_compile_definitions(Application PRIVATE STB_IMAGE_IMPLEMENTATION)
