cmake_minimum_required(VERSION 3.28)
project(rasterizer)

set(CMAKE_CXX_STANDARD 14)

add_executable(rasterizer
        main.cpp
        instanse.cpp
        instanse.h
        draw.h
        draw.cpp
        base.h
        base.cpp)
