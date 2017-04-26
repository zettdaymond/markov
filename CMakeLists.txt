cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(markov)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package (Threads)
set (markov_LIBS ${CMAKE_THREAD_LIBS_INIT})


option(BUILD_AS_MICROSERVICE "Build program as a http standalone server." ON)
option(GRAPHVIZ_LINK_LIBRARY "Using graphviz as the library, or use as a the process" OFF)
option(GRAPHVIZ_STATIC_LIBRARY "Include graphviz in the program. Awailable only if GRAPHVIZ_LINK_LIBRARY is enabled and MULTITHREADED_GRAPH_RENDERING is disabled" OFF)
option(MULTITHREADED_GRAPH_RENDERING "Using threads when rendering. This option is highly recommended to turn ON,
    if you use graphviz as the process instead as the library for rendering." ON)
option(VERBOSE_DEBUG_OUTPUT "Enable verbose debug outputs" OFF)
option(STATIC_BUILD "Link all possible libraries statically" OFF)


set(markov_SRC
    src/markov.cpp
    src/utils.cpp
    src/Result.hpp
    src/types.hpp
    src/lib_entry_point.cpp
)


#if graphviz library is not set, program will be builded with graphviz as a process support
if(GRAPHVIZ_LINK_LIBRARY)
    find_package(GraphViz REQUIRED)
    if(NOT GRAPHVIZ_STATIC_LIBRARY)
        list(APPEND markov_LIBS ${GRAPHVIZ_LIBRARIES})
        add_definitions(-DGRAPHVIZ_DYNAMIC_RENDERING=1)
    else()
        if(MULTITHREADED_GRAPH_RENDERING)
            message( WARNING "Using graphviz as a library with multithreading support is normal,
                but it only affect rendering preparation step and has no effect on rendering process itself
                (cause graphviz is not thread-safe)" )
        endif()
        set(GRAPHVIZ_STATIC_LIBS
                libgvplugin_core.a
                libgvplugin_dot_layout.a
                libdotgen.a
                libcommon.a
                libxdot.a
                libgvc.a
                libcgraph.a
                libcdt.a
                libpathplan.a
                libortho.a
                libpack.a
                liblabel.a)

        list(APPEND markov_LIBS ${GRAPHVIZ_STATIC_LIBS})
        add_definitions(-DGRAPHVIZ_STATIC_RENDERING=1)
    endif()
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
    if(WIN32)
        list(APPEND markov_LIBS libws2_32.a)
    endif()
else()
    set(markov_SRC ${markov_SRC} src/main_app.cpp)
endif()

if(STATIC_BUILD)
    list(APPEND CMAKE_EXE_LINKER_FLAGS -static)
    add_definitions(-static)
    # On MinGW there is a bug with static linking.
    # The only way to prevent this -> append to the end of last compilation string (link stage):
    # -Wl,-Bstatic -lwinpthread -lmicrohttpd -lWS2_32
    # Ensure that you have static version of winpthread and microhttpd libraries installed.
endif()

add_definitions(-DRAPIDJSON_HAS_STDSTRING=1 -DFMT_HEADER_ONLY=1 -std=c++14 -pthread)

add_executable(${PROJECT_NAME} ${markov_SRC})
target_link_libraries(${PROJECT_NAME} ${markov_LIBS})

message("# ---------------------------------------------------------")
message("# BUILD STATUS")
message("# ---------------------------------------------------------")
message("# BUILD_AS_MICROSERVICE \t\t${BUILD_AS_MICROSERVICE}")
message("# GRAPHVIZ_LINK_LIBRARY \t\t${GRAPHVIZ_LINK_LIBRARY}")
message("# GRAPHVIZ_STATIC_LIBRARY \t\t${GRAPHVIZ_STATIC_LIBRARY}")
message("# MULTITHREADED_GRAPH_RENDERING \t${MULTITHREADED_GRAPH_RENDERING}")
message("# VERBOSE_DEBUG_OUTPUT \t\t${VERBOSE_DEBUG_OUTPUT}")
message("# STATIC_BUILD \t\t${STATIC_BUILD}")
message("# ------------------------------------")
message(${markov_LIBS})
