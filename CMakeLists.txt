cmake_minimum_required(VERSION 2.8)

project(markov)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(RapidJSON REQUIRED)
find_package(fmt REQUIRED)
find_package(GraphViz REQUIRED)
add_definitions()

add_executable(${PROJECT_NAME} main.cpp markov.cpp utils.cpp)
target_link_libraries(markov libfmt.a ${GRAPHVIZ_LIBRARIES})

