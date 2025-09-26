

file(GLOB_RECURSE SRC_FILES
  "${CMAKE_SOURCE_DIR}/External/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/External/src/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

set(SRC
  ${SRC_FILES}
)

add_library(Application STATIC ${SRC})

target_include_directories( Application
  PRIVATE ${CMAKE_SOURCE_DIR}/External/include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
  target_link_libraries(Application PRIVATE
    ${CMAKE_SOURCE_DIR}/External/lib/glfw3.lib
    opengl32.lib user32.lib gdi32.lib shell32.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib
  )
endif()

target_compile_definitions(Application PRIVATE GLFW_STATIC)
target_compile_definitions(Application PRIVATE STB_IMAGE_IMPLEMENTATION)

set_property(TARGET Application PROPERTY CXX_STANDARD 20)
set_property(TARGET Application PROPERTY CXX_STANDARD_REQUIRED ON)
