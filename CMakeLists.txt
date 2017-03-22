cmake_minimum_required(VERSION 2.8)

project(markov)

find_package(RapidJSON REQUIRED)
add_definitions()

add_executable(${PROJECT_NAME} main.cpp markov.cpp)
