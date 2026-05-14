cmake_minimum_required(VERSION 3.10)
project(SingletonLogger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(logger_demo main.cpp)

find_package(Threads REQUIRED)
target_link_libraries(logger_demo PRIVATE Threads::Threads)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(logger_demo PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(logger_demo PRIVATE /W4)
endif()

