# Description #
**Markov** - is a micro-service (or console tool, depends on build options) that could determine optimal strategy in set of strategies, defined by probabilities and  revenues  matrices. It relies on "reccurent" method described in theory of controllable Markov chains.

# Dependencies #
* libmicrohttpd (OPTIONAL) - if you want to build Markov as micro-service.
* graphviz (OPTIONAL) - for strategy graph visualization
* cmake 
* c++14 compiler (gcc, clang)

# Platforms #
Markov may be build for Linux and Windows.

# Build #
## Build options ##

Markov may be build in different configurations, depends on ```cmake``` options. Be careful, different platforms have different restrictions.

* **BUILD_AS_MICROSERVICE**  - Build program as a http standalone server, or build as a console tool. *OFF*, by default.
* **GRAPHVIZ_LINK_LIBRARY**  - Using graphviz as the library, or use as a the process" *OFF*, by default
* **MULTITHREADED_GRAPH_RENDERING**  - Using threads when rendering. This option is highly recommended to turn ON, if you use graphviz as the process instead as the library for rendering. *OFF*, by default.
* **VERBOSE_DEBUG_OUTPUT** - Enable verbose debug outputs. *OFF*, by default.
* **STATIC_BUILD** - Link all possible libraries statically. *OFF*, by default


## Linux ##
Markov support static linking to libmicrohttpd for Linux, if you want so, please ensure that you have static version of libraries installed. There is no support for static linking to graphviz. It may be linked dynamically, or called as process - depending on build options.

clone the repo to your machine and create ```build``` folder
```
git clone https://bitbucket.org/zettdaymond/markov
cd markov
mkdir build
cd build
```
Run ```cmake``` with preffered options, throw ```-D*OPTION*=1```. See list of options above. For example:
```
cmake ..
make
```
## Windows ##
To build Markov for Windows you need **MSYS2** installed and configured. Markov support static linking to libmicrohttpd and graphviz for Windows, if you want so, please ensure that you have static version of libraries installed. There is no support for dynamic linking to graphviz, but there is a way to call it as a process.

clone the repo to your machine and create ```build``` folder
```
git clone https://bitbucket.org/zettdaymond/markov
cd markov
mkdir build
cd build
```
Run ```cmake``` with preffered options, throw ```-D*OPTION*=1```. See list of options below. For example:
```
cmake -G"MSYS Makefiles" ..
make
```