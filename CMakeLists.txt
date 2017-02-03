cmake_minimum_required (VERSION 3.7.20161201)
project (ket_cpp CXX)

set(serial 0.0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Doxygen)
if(DOXYGEN_FOUND)
	message(STATUS "Generating a rule to build documentation by doxygen")
	if(DOXYGEN_DOT_FOUND)
		set(DOXYGEN_DOT_FOUND_YESNO YES)
	else(DOXYGEN_DOT_FOUND)
		set(DOXYGEN_DOT_FOUND_YESNO NO)
	endif(DOXYGEN_DOT_FOUND)

	set(KETCPP_DOC_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/")
	set(KETCPP_DOC_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/main.cc ${CMAKE_CURRENT_SOURCE_DIR}/src/")
	configure_file(Doxyfile.in Doxyfile)

	add_custom_target(doc ${DOXYGEN_EXECUTABLE})
endif(DOXYGEN_FOUND)

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

set_property(TARGET ket_cpp run_tests PROPERTY CXX_STANDARD 17)
set_property(TARGET ket_cpp run_tests PROPERTY CXX_STANDARD_REQUIRED TRUE)

install(TARGETS ket_cpp DESTINATION bin COMPONENT runtime)
