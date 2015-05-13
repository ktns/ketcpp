cmake_minimum_required (VERSION 3.1)
project (ket_cpp CXX)

add_executable (ket_cpp test.cc)

set_property(TARGET ket_cpp PROPERTY CXX_STANDARD 14)
set_property(TARGET ket_cpp PROPERTY CXX_STANDARD_REQUIRED TRUE)
