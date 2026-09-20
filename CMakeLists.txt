cmake_minimum_required(VERSION 3.16)

project(rm_reprojection
        VERSION 1.0.0
        DESCRIPTION "理想针孔相机重投影 (C++17)"
        LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(reprojection
  src/main.cpp
  src/reprojection.cpp
)

target_include_directories(reprojection PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(reprojection PRIVATE -Wall -Wextra -Wpedantic)
