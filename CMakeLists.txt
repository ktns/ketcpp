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


	file(GLOB_RECURSE KETCPP_DOC_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
	list(INSERT KETCPP_DOC_INPUT 0 "${CMAKE_CURRENT_SOURCE_DIR}/main.cc")
	set(KETCPP_DOC_OUTPUT "${KETCPP_DOC_DIR}/html/index.html")
	set(KETCPP_DOC_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/")

	configure_file(Doxyfile.in Doxyfile)

	add_custom_command(OUTPUT ${KETCPP_DOC_OUTPUT} DEPENDS ${KETCPP_DOC_INPUT} COMMAND ${DOXYGEN_EXECUTABLE})
	add_custom_target(doc DEPENDS ${KETCPP_DOC_OUTPUT})
endif(DOXYGEN_FOUND)

find_package(PkgConfig REQUIRED)

find_package(Eigen3)
if(EIGEN3_FOUND)
	include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})
else(EIGEN3_FOUND)
	pkg_check_modules(EIGEN3_PC eigen3)
	if(EIGEN3_PC_FOUND)
		set(EIGEN3_INCLUDE_DIR ${EIGEN3_PC_INCLUDE_DIRS})
		include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})
	endif(EIGEN3_PC_FOUND)
endif(EIGEN3_FOUND)

pkg_check_modules(LIBINT2 libint2)
if(LIBINT2_FOUND)
	include_directories(SYSTEM ${LIBINT2_INCLUDE_DIRS})
	link_libraries(int2)
	if(LIBINT2_VERSION VERSION_LESS "2.2")
		MESSAGE(WARNING "Not using libint2 ${LIBINT2_VERSION} < 2.2")
		unset(LIBINT2_FOUND CACHE)
	endif()
endif(LIBINT2_FOUND)

configure_file(
	${PROJECT_SOURCE_DIR}/src/config/ketcpp_config.h.in
	${PROJECT_SOURCE_DIR}/src/config/ketcpp_config.h)

file(GLOB_RECURSE SRCS src/*.cc)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_library(ketcpp STATIC ${SRCS})

add_executable (ket_cpp main.cc)
target_link_libraries(ket_cpp PRIVATE ketcpp)

file(GLOB_RECURSE TEST_SRCS test/*.cc)

enable_testing()
add_executable (run_tests ${TEST_SRCS})
target_link_libraries(run_tests PRIVATE ketcpp)
target_include_directories(run_tests PRIVATE ${PROJECT_SOURCE_DIR}/test/)
target_include_directories(run_tests SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/extlib/bandit)
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
	target_compile_options(run_tests PUBLIC "-Wsuggest-override")
endif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
add_test(NAME run_tests COMMAND $<TARGET_FILE:run_tests> --reporter=spec)
set(KETCPP_TEST_FIXTURE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/test/fixture")
target_include_directories(run_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/test/")
target_include_directories(run_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/test/")
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/test/config/ketcpp_test_config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/test/config/ketcpp_test_config.h)

set_property(TARGET ketcpp ket_cpp run_tests PROPERTY CXX_STANDARD 17)
set_property(TARGET ketcpp ket_cpp run_tests PROPERTY CXX_STANDARD_REQUIRED TRUE)

set(ENABLE_COTIRE YES CACHE BOOL "Use cotire in the build or not")
if("${COTIRE_CMAKE_MODULE_VERSION}" VERSION_GREATER "1.7.2.99" AND "${ENABLE_COTIRE}")
	cotire(run_tests)
	cotire(ket_cpp)
endif("${COTIRE_CMAKE_MODULE_VERSION}" VERSION_GREATER "1.7.2.99" AND "${ENABLE_COTIRE}")

list(APPEND LCOV_REMOVE_PATTERNS "'${PROJECT_SOURCE_DIR}/extlib/*'" "'${PROJECT_SOURCE_DIR}/test/*'")
add_coverage(run_tests)
coverage_evaluate()

install(TARGETS ket_cpp DESTINATION bin COMPONENT runtime)
