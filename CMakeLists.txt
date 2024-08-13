cmake_minimum_required(VERSION 3.30.1)

if(${BUILD_TARGET} STREQUAL "client")
  project(gossip_client CXX)
elseif(${BUILD_TARGET} STREQUAL "server")
  project(gossip_server CXX)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

if(${BUILD_TARGET} STREQUAL "client")
  find_package(Curses REQUIRED)
endif()


if(${BUILD_TARGET} STREQUAL "client")
  add_executable(${PROJECT_NAME}
    src/client.cpp
  )
elseif(${BUILD_TARGET} STREQUAL "server")
  add_executable(${PROJECT_NAME}
    src/server.cpp
  )
endif()

include_directories(${PROJECT_NAME}
  inc/
)

if(${BUILD_TARGET} STREQUAL "client")
  include_directories(${PROJECT_NAME}
    ${CURSES_INCLUDE_DIR}
  )
endif()

if(${BUILD_TARGET} STREQUAL "client")
  target_link_libraries(${PROJECT_NAME}
    ${CURSES_LIBRARIES}
  )
endif()
