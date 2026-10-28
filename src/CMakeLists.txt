cmake_minimum_required(VERSION 3.10)
project(lab3 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_executable(program Main.c)