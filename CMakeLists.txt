cmake_minimum_required(VERSION 3.24)
project(adt_example C)

set(CMAKE_C_STANDARD 23)

add_executable(adt_example main.c example/deque.h example/deque.c)
