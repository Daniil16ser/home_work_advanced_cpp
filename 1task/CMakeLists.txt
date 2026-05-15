cmake_minimum_required(VERSION 3.10)

cmake_minimum_required(VERSION 3.10)

project(util_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(util_test src/main.cpp src/lib/Group.cpp src/lib/Util.cpp)

target_include_directories(util_test PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/lib)

target_compile_options(util_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)