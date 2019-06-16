cmake_minimum_required(VERSION 3.0)

project(gameutils)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

file(GLOB_RECURSE source ../source/*)
#add_library(gameutils ${source})
add_executable(gutest ${source})
#set_target_properties(gameutils PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(gutest PROPERTIES LINKER_LANGUAGE CXX)

#set_property(TARGET gameutils PROPERTY CXX_STANDARD 17)
#set_property(TARGET gameutils PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET gutest PROPERTY CXX_STANDARD 17)
set_property(TARGET gutest PROPERTY CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# ---GLFW---

# disable GLFW docs, tests and examples:
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(../external/glfw-3.2.1 ./bin/glfw)
include_directories(../external/glfw-3.2.1)


# ---GLM---
add_subdirectory(../external/glm-0.9.9.5/glm ./bin/glm)
include_directories(../external/glm-0.9.9.5/glm)


# ---GLAD---
add_library(glad ../external/glad/include/glad/glad.h ../external/glad/src/glad.c)
target_include_directories(glad PUBLIC ../external/glad/include/)


# -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
set(CMAKE_CXX_FLAGS  "-no-pie -O3")

#target_link_libraries(gameutils glad glfw glm)
#target_include_directories(gameutils PUBLIC source/)

#set_target_properties(gutest PROPERTIES SUFFIX ".html")
#set_target_properties(gutest PROPERTIES LINK_FLAGS "-s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm' --preload-file assets")

target_link_libraries(gutest glad glfw glm)
