cmake_minimum_required (VERSION 3.1)
project (ket_cpp CXX)

set(serial 0.0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(PkgConfig)
pkg_check_modules(EIGEN3_PC eigen3)
if(EIGEN3_PC_FOUND)
  set(EIGEN3_INCLUDE_DIR ${EIGEN3_PC_INCLUDE_DIRS})
endif(EIGEN3_PC_FOUND)
find_package(Eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})

file(GLOB_RECURSE SRCS src/*.cc)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/extlib/infix_iterator/)

add_executable (ket_cpp main.cc ${SRCS})

file(GLOB_RECURSE TEST_SRCS test/*.cc)

enable_testing()
add_executable (run_tests ${SRCS} ${TEST_SRCS})
target_include_directories(run_tests PRIVATE extlib/bandit)
add_test(NAME run_tests COMMAND $<TARGET_FILE:run_tests> --reporter=spec)

set_property(TARGET ket_cpp run_tests PROPERTY CXX_STANDARD 14)
set_property(TARGET ket_cpp run_tests PROPERTY CXX_STANDARD_REQUIRED TRUE)

install(TARGETS ket_cpp DESTINATION bin COMPONENT runtime)
