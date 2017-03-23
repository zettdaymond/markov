cmake_minimum_required(VERSION 2.8)

project(markov)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

option(BUILD_AS_MICROSERVICE "Build program as http standalone server." OFF)

find_package(RapidJSON REQUIRED)
find_package(fmt REQUIRED)
find_package(GraphViz REQUIRED)

set(markov_SRC markov.cpp utils.cpp)

if(BUILD_AS_MICROSERVICE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(LIBMICROHTTPD REQUIRED libmicrohttpd)
    set(markov_SRC ${markov_SRC} main_silicon.cpp)
else()
    set(markov_SRC ${markov_SRC} main.cpp)
endif()


add_executable(${PROJECT_NAME} ${markov_SRC})

target_link_libraries(${PROJECT_NAME} libfmt.a ${GRAPHVIZ_LIBRARIES})

if(BUILD_AS_MICROSERVICE)
    target_link_libraries(${PROJECT_NAME} libfmt.a ${GRAPHVIZ_LIBRARIES} ${LIBMICROHTTPD_LIBRARIES})
endif()
