cmake_minimum_required(VERSION 3.30.1)

project(gossip CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME}
  src/server.cpp
  src/tea.cpp
)

include_directories(${PROJECT_NAME}
  inc/
)
