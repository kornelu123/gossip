cmake_minimum_required(VERSION 3.30.1)

project(tests CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
  ../src/files.cpp
  tests.cpp
)
