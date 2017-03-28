cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(markov)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

option(BUILD_AS_MICROSERVICE "Build program as a http standalone server." OFF)
option(GRAPHVIZ_LINK_LIBRARY "Using graphviz as a library, or throw executing processes" ON)

set(markov_SRC
    src/markov.cpp
    src/utils.cpp
    src/Result.hpp
    src/types.hpp
    src/lib_entry_point.cpp
)

set (markov_LIBS "")

#if graphviz library is not set, program will be builded with process support
if(GRAPHVIZ_LINK_LIBRARY)
    find_package(GraphViz REQUIRED)
    list(APPEND markov_LIBS ${GRAPHVIZ_LIBRARIES})
    add_definitions(-DGRAPHVIZ_DYNAMIC_RENDERING=1)
else()
    add_subdirectory(thirdparty/tiny-process-library)
    list(APPEND markov_LIBS tiny-process-library)
endif()


if(BUILD_AS_MICROSERVICE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(LIBMICROHTTPD REQUIRED libmicrohttpd)
    set(markov_SRC ${markov_SRC} src/main_silicon.cpp)
    list(APPEND markov_LIBS ${LIBMICROHTTPD_LIBRARIES})
else()
    set(markov_SRC ${markov_SRC} src/main_app.cpp)
endif()


add_definitions(-DRAPIDJSON_HAS_STDSTRING=1 -DFMT_HEADER_ONLY=1 -std=c++14 -static)


add_executable(${PROJECT_NAME} ${markov_SRC})
target_link_libraries(${PROJECT_NAME} ${markov_LIBS})

message("# ------------------------------------")
message("# BUILD STATUS")
message("  BUILD_AS_MICROSERVICE ${BUILD_AS_MICROSERVICE}")
message("  GRAPHVIZ_LINK_LIBRARY ${GRAPHVIZ_LINK_LIBRARY}")
message("# ------------------------------------")
