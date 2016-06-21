cmake_minimum_required(VERSION 3.3)
project(Thesis)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -s")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS -static)         # static linking
link_libraries(ws2_32 wsock32 boost_system) # these libraries are used for the server/client communcation
include_directories(header_files)           # include all the header files from the header_files directory and sub-dirs
include_directories(C:/MinGW/MinGW/include) # include boost libraries

# Create a file list with our to-be-compiled source files
file(GLOB_RECURSE HEADER_AND_SOURCE_FILES # [GLOB_RECURSE]: looks in sub-dirs, [GLOB]: doesn't look in sub-dirs
        main.cpp              # this file contains a main method
        "header_files/*.h"    # we want to include all header files from the header_files directory
        "header_files/*.hpp"  # we want to include all header files from the header_files directory
        "source_files/*.cpp") # we want to include all source files from the source_files directory
add_executable(Thesis ${HEADER_AND_SOURCE_FILES} source_files/QueryGraph.cpp header_files/QueryGraph.h)