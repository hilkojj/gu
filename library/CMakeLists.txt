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
target_link_libraries(gameutils PUBLIC glad)


# ---GLFW---
if(NOT DEFINED EMSCRIPTEN)
    # disable GLFW docs, tests and examples:
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(../external/glfw ./bin/glfw)
    include_directories(../external/glfw/include)

    target_link_libraries(gameutils PUBLIC glfw)
endif()


# ---GLM---
set(GLM_TEST_ENABLE OFF)
add_subdirectory(../external/glm/glm ./bin/glm)
include_directories(../external/glm/glm)
target_link_libraries(gameutils PUBLIC glm)


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
target_link_libraries(gameutils PUBLIC imgui)



# ---ZLIB---

set(ZLIB_DIR ../external/zlib)
file(GLOB ZLIB_SOURCES ${ZLIB_DIR}/*.c)
file(GLOB ZLIB_HEADERS ${ZLIB_DIR}/*.h)

add_library(zlib ${ZLIB_SOURCES} ${ZLIB_HEADERS})

target_include_directories(zlib PUBLIC ${ZLIB_DIR})
target_link_libraries(gameutils PUBLIC zlib)



# ---TinyGLTF---
file(DOWNLOAD https://raw.githubusercontent.com/syoyo/tinygltf/7e83ef9fb44d8150d69e3d70067146f3746db917/tiny_gltf.h ${CMAKE_CURRENT_LIST_DIR}/../external/tiny_gltf.h
        STATUS DOWNLOAD_STATUS)

# Separate the returned status code, and error message.
list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
# Check if download was successful.
if(${STATUS_CODE} EQUAL 0)
    message(STATUS "tiny_gltf.h download completed successfully!")
else()
    # Exit CMake if the download failed, printing the error message.
    message(FATAL_ERROR "Error occurred during tiny_gltf.h download: ${ERROR_MESSAGE}")
endif()


# ---Bullet2---
add_subdirectory(../external/bullet2/src ./bin/bullet2)
target_link_libraries(gameutils PUBLIC Bullet)


target_include_directories(gameutils PUBLIC ../source/)
