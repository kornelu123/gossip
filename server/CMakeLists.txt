cmake_minimum_required(VERSION 3.24)

project(gossip_server)

set(CMAKE_CXX_STANDARD_17)

include_directories(
  ${PROJECT_SOURCE_DIR}/inc
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/../inc
  ${PROJECT_SOURCE_DIR}/../src
)

file(GLOB ALL_SRC
  ${PROJECT_SOURCE_DIR}/inc/*.h
  ${PROJECT_SOURCE_DIR}/src/*.c
  ${PROJECT_SOURCE_DIR}/../inc/*.h
  ${PROJECT_SOURCE_DIR}/../src/*.c
)

add_compile_options(-Wall -Wextra)
 
add_executable(gossip_server ${ALL_SRC})

target_link_libraries(gossip_server)
