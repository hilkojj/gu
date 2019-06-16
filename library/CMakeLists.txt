cmake_minimum_required(VERSION 3.0)

project(gameutils)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

file(GLOB_RECURSE source ../source/*)
add_library(gameutils ${source})
set_target_properties(gameutils PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET gameutils PROPERTY CXX_STANDARD 17)
set_property(TARGET gameutils PROPERTY CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# ---GLAD---
add_library(glad ../external/glad/include/glad/glad.h ../external/glad/src/glad.c)
target_include_directories(glad PUBLIC ../external/glad/include/)
target_link_libraries(gameutils glad)


# ---GLFW---
if(NOT DEFINED EMSCRIPTEN)
    # disable GLFW docs, tests and examples:
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(../external/glfw-3.2.1 ./bin/glfw)
    include_directories(../external/glfw-3.2.1)

    target_link_libraries(gameutils glfw)
endif()


# ---GLM---
add_subdirectory(../external/glm-0.9.9.5/glm ./bin/glm)
include_directories(../external/glm-0.9.9.5/glm)
target_link_libraries(gameutils glm)

target_include_directories(gameutils PUBLIC ../source/)
