cmake_minimum_required(VERSION 3.22)
project(SSTree)

set(CMAKE_CXX_STANDARD 17)

find_package( OpenCV REQUIRED )
add_executable(SSTree main.cpp SSTree.h SSTree.cpp)
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( SSTree ${OpenCV_LIBS} )