project(main)

cmake_minimum_required(VERSION 3.8)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)
