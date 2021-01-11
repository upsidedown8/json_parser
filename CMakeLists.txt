cmake_minimum_required(VERSION 3.13)
project(json_parser_project)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

include_directories(
    src/
    inc/
    /usr/include/
)

set(
    SRC_FILES
    src/ast.cpp
    src/json_parser.cpp
    src/parser.cpp
)

add_library(
   json_parser
   ${SRC_FILES}
)

add_executable(
    json_parser_exec
    src/main.cpp
)
target_link_libraries(
    json_parser_exec
    json_parser
)