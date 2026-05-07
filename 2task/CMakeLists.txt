cmake_minimum_required(VERSION 3.16)

project(TypeListDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(typelist_test main.cpp)

target_include_directories(typelist_test PRIVATE ${CMAKE_SOURCE_DIR})

target_compile_options(typelist_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)