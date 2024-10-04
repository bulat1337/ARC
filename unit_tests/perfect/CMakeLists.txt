cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data/")
add_definitions(-DTEST_DATA_DIR=\"${TEST_DATA_DIR}\")

find_package(GTest REQUIRED)

set(COMPILE_OPTIONS
	-O2
	-fstack-protector
	-fPIE
	-fstrict-overflow
	-Wformat-security
	-Wformat=2
	-Wall
	-Wextra
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wcast-align
	-Wpointer-arith
	-Wunreachable-code
)

add_executable(perfect_tests
	perfect_tests.cpp
	main.cpp
)

target_compile_options(perfect_tests PRIVATE ${COMPILE_OPTIONS})

target_link_libraries(perfect_tests
	GTest::GTest
	GTest::Main
)

target_include_directories(perfect_tests PRIVATE
	${CMAKE_SOURCE_DIR}/unit_tests/test_utils/include
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/utils/include
)

if(ENABLE_BD_TESTS)
    target_compile_definitions(perfect_tests PRIVATE BD_TESTS)
endif()

enable_testing()
add_test(NAME PerfectTests COMMAND perfect_tests)
