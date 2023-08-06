cmake_minimum_required(VERSION 3.24)

project(gossip_client)

set(CMAKE_CXX_STANDARD_17)

include_directories(
  ${PROJECT_SOURCE_DIR}/inc
  ${PROJECT_SOURCE_DIR}/src
)

file(GLOB ALL_SRC
  ${PROJECT_SOURCE_DIR}/inc/*.h
  ${PROJECT_SOURCE_DIR}/src/*.c
)
add_executable(gossip_client ${ALL_SRC})

target_link_libraries(gossip_client)
