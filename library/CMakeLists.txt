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

if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/../external/glad)
    # generate glad-opengl-loader-stuff
    execute_process(COMMAND python -m pip install --user glad)
    execute_process(COMMAND python -m glad --generator=c --extensions=GL_EXT_texture_compression_s3tc --out-path=../external/glad WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()

add_library(glad ../external/glad/include/glad/glad.h ../external/glad/src/glad.c)
target_include_directories(glad PUBLIC ../external/glad/include/)
target_link_libraries(gameutils glad)


# ---GLFW---
if(NOT DEFINED EMSCRIPTEN)
    # disable GLFW docs, tests and examples:
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(../external/glfw ./bin/glfw)
    include_directories(../external/glfw/include)

    target_link_libraries(gameutils glfw)
endif()


# ---GLM---
set(GLM_TEST_ENABLE OFF)
add_subdirectory(../external/glm/glm ./bin/glm)
include_directories(../external/glm/glm)
target_link_libraries(gameutils glm)


# ---OpenAL soft---
if (NOT DEFINED EMSCRIPTEN)
    set(ALSOFT_EXAMPLES OFF)
    set(ALSOFT_TESTS OFF)
    set(ALSOFT_UTILS OFF)

    add_subdirectory(../external/openal-soft ./bin/openal)
    include_directories(../external/openal-soft/include)
    target_link_libraries(gameutils OpenAL)
endif ()


# ---IMGUI---

set(IMGUI_DIR ../external/imgui)
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

target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_link_libraries(imgui glfw)
target_link_libraries(gameutils imgui)



# ---ZLIB---

set(ZLIB_DIR ../external/zlib)
file(GLOB ZLIB_SOURCES ${ZLIB_DIR}/*.c)
file(GLOB ZLIB_HEADERS ${ZLIB_DIR}/*.h)

add_library(zlib ${ZLIB_SOURCES} ${ZLIB_HEADERS})

target_include_directories(zlib PUBLIC ${ZLIB_DIR})
target_link_libraries(gameutils zlib)

target_include_directories(gameutils PUBLIC ../source/)
