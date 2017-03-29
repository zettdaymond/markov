cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(markov)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package (Threads)
set (markov_LIBS ${CMAKE_THREAD_LIBS_INIT})


option(BUILD_AS_MICROSERVICE "Build program as a http standalone server." OFF)
option(GRAPHVIZ_LINK_LIBRARY "Using graphviz as the library, or use as a the process" OFF)
option(MULTITHREADED_GRAPH_RENDERING "Using threads when rendering. This option is highly recommended to turn ON,
    if you use graphviz as the process instead as the library for rendering." OFF)
option(VERBOSE_DEBUG_OUTPUT "Enable verbose debug outputs" OFF)


set(markov_SRC
    src/markov.cpp
    src/utils.cpp
    src/Result.hpp
    src/types.hpp
    src/lib_entry_point.cpp
)


#if graphviz library is not set, program will be builded with process support
if(GRAPHVIZ_LINK_LIBRARY)
    find_package(GraphViz REQUIRED)
    list(APPEND markov_LIBS ${GRAPHVIZ_LIBRARIES})
    add_definitions(-DGRAPHVIZ_DYNAMIC_RENDERING=1)
else()
    add_subdirectory(thirdparty/tiny-process-library)
    list(APPEND markov_LIBS tiny-process-library)
endif()


if(VERBOSE_DEBUG_OUTPUT)
    add_definitions(-DENABLE_DEBUG_OUTPUT=1)
endif()

if(MULTITHREADED_GRAPH_RENDERING)
    add_definitions(-DMULTITHREADED_RENDERING=1)
endif()

if(BUILD_AS_MICROSERVICE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(LIBMICROHTTPD REQUIRED libmicrohttpd)
    set(markov_SRC ${markov_SRC} src/main_silicon.cpp)
    list(APPEND markov_LIBS ${LIBMICROHTTPD_LIBRARIES})
else()
    set(markov_SRC ${markov_SRC} src/main_app.cpp)
endif()


add_definitions(-DRAPIDJSON_HAS_STDSTRING=1 -DFMT_HEADER_ONLY=1 -std=c++14 -static -pthread)


add_executable(${PROJECT_NAME} ${markov_SRC})
target_link_libraries(${PROJECT_NAME} ${markov_LIBS})

message("# ---------------------------------------------------------")
message("# BUILD STATUS")
message("# ---------------------------------------------------------")
message("# BUILD_AS_MICROSERVICE \t\t${BUILD_AS_MICROSERVICE}")
message("# GRAPHVIZ_LINK_LIBRARY \t\t${GRAPHVIZ_LINK_LIBRARY}")
message("# MULTITHREADED_GRAPH_RENDERING \t${MULTITHREADED_GRAPH_RENDERING}")
message("# VERBOSE_DEBUG_OUTPUT \t\t${VERBOSE_DEBUG_OUTPUT}")
message("# ------------------------------------")
