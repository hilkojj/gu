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


# ---IMGUI---

set(IMGUI_DIR ../external/imgui-1.71)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp)
file(GLOB IMGUI_HEADERS ${IMGUI_DIR}/*.h)

add_library(imgui ${IMGUI_SOURCES} ${IMGUI_HEADERS}
    ${IMGUI_DIR}/examples/imgui_impl_glfw.h
    ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.h
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)

if (NOT DEFINED EMSCRIPTEN)
    add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD) # tell IMGUI that we use glad on desktop
    target_link_libraries(imgui glad)
else ()
    add_definitions(-DIMGUI_IMPL_OPENGL_ES3)
endif ()

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)

target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_link_libraries(imgui glfw)
target_link_libraries(gameutils imgui)



# ---Node-editor---
set(NODE_EDITOR_DIR ../external/node-editor)
file(GLOB NODE_EDITOR_SOURCES ${NODE_EDITOR_DIR}/*.cpp)
file(GLOB NODE_EDITOR_HEADERS ${NODE_EDITOR_DIR}/*.h)

add_library(node-editor ${NODE_EDITOR_SOURCES} ${NODE_EDITOR_HEADERS})

target_include_directories(node-editor PUBLIC ${NODE_EDITOR_DIR})
target_link_libraries(node-editor imgui)
target_link_libraries(gameutils node-editor)


target_include_directories(gameutils PUBLIC ../source/)
