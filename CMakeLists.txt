cmake_minimum_required(VERSION 3.16)
project(CMagic
        VERSION 0.1.0
        DESCRIPTION "Portable C library of utilities and data structures"
        HOMEPAGE_URL "https://github.com/mateusz-olczyk/CMagic"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

message(STATUS "Hello! This is ${PROJECT_NAME} v.${PROJECT_VERSION}: ${PROJECT_DESCRIPTION}.")
