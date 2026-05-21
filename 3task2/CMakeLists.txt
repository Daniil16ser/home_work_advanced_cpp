cmake_minimum_required(VERSION 3.10)
project(TypeMapExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)

# Добавьте эту строку - указывает, где искать заголовочные файлы
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})