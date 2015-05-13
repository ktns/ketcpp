cmake_minimum_required (VERSION 3.1)
project (ket_cpp CXX)

set(serial 0.0)

file(GLOB_RECURSE SRCS src/*.cc)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable (ket_cpp main.cc ${SRCS})

file(GLOB_RECURSE TEST_SRCS test/*.cc)

enable_testing()
add_executable (run_tests ${SRCS} ${TEST_SRCS})
target_include_directories(run_tests PRIVATE extlib/igloo)
add_test(NAME run_tests COMMAND $<TARGET_FILE:run_tests>)

set_property(TARGET ket_cpp run_tests PROPERTY CXX_STANDARD 14)
set_property(TARGET ket_cpp run_tests PROPERTY CXX_STANDARD_REQUIRED TRUE)
