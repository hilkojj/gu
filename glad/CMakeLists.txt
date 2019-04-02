cmake_minimum_required(VERSION 3.0)
project(Glad)

add_library(glad ../external/glad/include/glad/glad.h ../external/glad/src/glad.c)
target_include_directories(glad PUBLIC ../external/glad/include/)
