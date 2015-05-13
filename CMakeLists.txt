cmake_minimum_required (VERSION 3.1)
project (ket_cpp CXX)

set(serial 0.0)

file(GLOB_RECURSE SRCS src/*.cc)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable (ket_cpp main.cc ${SRCS})

set_property(TARGET ket_cpp PROPERTY CXX_STANDARD 14)
set_property(TARGET ket_cpp PROPERTY CXX_STANDARD_REQUIRED TRUE)
