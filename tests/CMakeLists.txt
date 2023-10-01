cmake_minimum_required(VERSION 3.24)

project(tests)

set(CMAKE_CXX_STANDARD_17)

include_directories(
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/../inc
    ${PROJECT_SOURCE_DIR}/../src
    ${PROJECT_SOURCE_DIR}/inc
)

file(GLOB ALL_SRC
    ${PROJECT_SOURCE_DIR}/*.h
    ${PROJECT_SOURCE_DIR}/*.c
    ${PROJECT_SOURCE_DIR}/../inc/*.h
    ${PROJECT_SOURCE_DIR}/../src/*.c
    ${PROJECT_SOURCE_DIR}/inc/*.h
)

add_compile_options(-Wall -Wextra -fstack-protector-all)

add_executable(tests ${ALL_SRC})

target_link_libraries(tests)
