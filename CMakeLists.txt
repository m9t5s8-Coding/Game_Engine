
cmake_minimum_required(VERSION 3.20)



if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_compile_options(/utf-8)

project ("Game_Engine")


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


add_subdirectory(${CMAKE_SOURCE_DIR}/External/glfw)

add_subdirectory("Application")
add_subdirectory("Sandbox")




set_property(TARGET Sandbox PROPERTY CXX_STANDARD 20)
set_property(TARGET Sandbox PROPERTY CXX_STANDARD_REQUIRED ON)
