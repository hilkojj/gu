cmake_minimum_required(VERSION 3.0)

project(gameutils_html)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

file(GLOB_RECURSE source ../source/*)
add_executable(gutest ${source})
set_target_properties(gutest PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET gutest PROPERTY CXX_STANDARD 17)
set_property(TARGET gutest PROPERTY CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# ---GLM---
add_subdirectory(../external/glm-0.9.9.5/glm ./bin/glm)
include_directories(../core/external/glm-0.9.9.5/glm)


# ---GLAD---
add_library(glad ../external/glad/include/glad/glad.h ../external/glad/src/glad.c)
target_include_directories(glad PUBLIC ../external/glad/include/)

set_target_properties(gutest PROPERTIES SUFFIX ".html")
set_target_properties(gutest PROPERTIES LINK_FLAGS "-s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm' --preload-file ../assets")

target_link_libraries(gutest glad glm)
