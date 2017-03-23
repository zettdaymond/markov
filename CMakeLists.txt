cmake_minimum_required(VERSION 2.8)

project(markov)

find_package(RapidJSON REQUIRED)
find_package(fmt REQUIRED)
add_definitions()

add_executable(${PROJECT_NAME} main.cpp markov.cpp)
target_link_libraries(markov libfmt.a)

