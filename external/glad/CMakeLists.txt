cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME glad)

project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC include/glad/glad.h src/glad.c)
target_include_directories(${PROJECT_NAME} PUBLIC include/)