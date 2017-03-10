cmake_minimum_required (VERSION 3.7.20161201)
project (ket_cpp CXX)

set(serial 0.0)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(COTIRE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extlib/bandit/cmake/cotire/CMake/cotire.cmake)
if(EXISTS ${COTIRE_PATH})
	include(${COTIRE_PATH})
endif(EXISTS ${COTIRE_PATH})

set(CODECOV_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extlib/codecov/cmake/)
list(APPEND CMAKE_MODULE_PATH ${CODECOV_MODULE_PATH})
find_package(codecov)

set(DOXYGEN_USE_MATHJAX NO CACHE BOOL "Whether doxygen will generate an HTML document that use MathJax")
find_package(Doxygen)
if(DOXYGEN_FOUND)
	message(STATUS "Generating a rule to build documentation by doxygen")
	if(DOXYGEN_DOT_FOUND)
		set(DOXYGEN_DOT_FOUND_YESNO YES)
	else(DOXYGEN_DOT_FOUND)
		set(DOXYGEN_DOT_FOUND_YESNO NO)
	endif(DOXYGEN_DOT_FOUND)

	if(DOXYGEN_USE_MATHJAX)
		set(DOXYGEN_USE_MATHJAX_YESNO YES)
		find_package(MathJax)
		if(MATHJAX_FOUND)
			set(MATHJAX_PATH_OR_URL ${MATHJAX_PATH})
		else(MATHJAX_FOUND)
			set(MATHJAX_PATH_OR_URL "http://cdn.mathjax.org/mathjax/latest")
		endif(MATHJAX_FOUND)
	else(DOXYGEN_USE_MATHJAX)
		set(DOXYGEN_USE_MATHJAX_YESNO NO)
	endif(DOXYGEN_USE_MATHJAX)

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
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})

file(GLOB_RECURSE SRCS src/*.cc)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable (ket_cpp main.cc ${SRCS})

file(GLOB_RECURSE TEST_SRCS test/*.cc)

enable_testing()
add_executable (run_tests ${SRCS} ${TEST_SRCS})
target_include_directories(run_tests PRIVATE ${PROJECT_SOURCE_DIR}/test/)
target_include_directories(run_tests SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/extlib/bandit)
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
	target_compile_options(run_tests PUBLIC "-Wsuggest-override")
endif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
add_test(NAME run_tests COMMAND $<TARGET_FILE:run_tests> --reporter=spec)

set_property(TARGET ket_cpp run_tests PROPERTY CXX_STANDARD 17)
set_property(TARGET ket_cpp run_tests PROPERTY CXX_STANDARD_REQUIRED TRUE)

list(APPEND LCOV_REMOVE_PATTERNS "'${PROJECT_SOURCE_DIR}/extlib/*'" "'${PROJECT_SOURCE_DIR}/test/*'")
add_coverage(run_tests)
coverage_evaluate()

install(TARGETS ket_cpp DESTINATION bin COMPONENT runtime)
