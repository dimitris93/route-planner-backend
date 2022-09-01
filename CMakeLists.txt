cmake_minimum_required(VERSION 3.3)
project(routing_engine_backend)

# Include our headers
include_directories(header_files)

# We need to include the Boost library.
# Wn Windows, I am using a MinGW distribution that has Boost included.
# It can be downloaded from https://nuwen.net/mingw.html and then copied over to your C:/ drive.
set(Boost_INCLUDE_DIR C:/Users/Dimitris/MinGW/include)
set(Boost_LIBRARY_DIR C:/Users/Dimitris/MinGW/lib)
#find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# These libraries are used for the server/client communication from the 'websocketpp' header only library
link_libraries(ws2_32 wsock32)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3") # flags for release
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17") # flags for debug
set(CMAKE_EXE_LINKER_FLAGS -static)   # static linking

# Add our source files
file(GLOB_RECURSE source_files  main.cpp  "source_files/*.cpp") # create a list of files
add_executable(routing_engine_backend ${source_files})


# Compile for linux:
# g++ -std=c++11 main.cpp source_files/*.cpp -o server_exe -I./header_files -lboost_system