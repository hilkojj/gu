cmake_minimum_required (VERSION 3.0)
project (game)
set(EXECUTABLE_OUTPUT_PATH "bin")
add_executable(game main.cpp)

find_package(OpenGL REQUIRED)



# ---GLFW---

# disable GLFW docs, tests and examples:
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(./external/glfw-3.2.1)
include_directories(./external/glfw-3.2.1)
target_link_libraries(game glfw)



# -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
set(CMAKE_CXX_FLAGS  "-no-pie")
