# To run tests:
#	env CTEST_OUTPUT_ON_FAILURE=1 cmake --build ./ --target test

cmake_minimum_required(VERSION 3.13)

project(Main)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ENABLE_LOGGING "Enable logging" OFF)

add_executable(arc.x ./main.cpp)

target_include_directories(arc.x PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

if(ENABLE_LOGGING)
	target_compile_definitions(arc.x PRIVATE ENABLE_LOGGING)
endif()

target_compile_options(arc.x PRIVATE
	-D_DEBUG
	-ggdb3
	-O2
	-Wall
	-Wextra
	-Weffc++
	-Wc++14-compat
	-Wmissing-declarations
	-Wcast-align
	-Wcast-qual
	-Wchar-subscripts
	-Wconversion
	-Wctor-dtor-privacy
	-Wempty-body
	-Wfloat-equal
	-Wformat-nonliteral
	-Wformat-security
	-Wformat=2
	-Winline
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wpacked
	-Wpointer-arith
	-Winit-self
	-Wredundant-decls
	-Wshadow
	-Wsign-conversion
	-Wsign-promo
	-Wstrict-overflow=2
	-Wsuggest-override
	-Wswitch-default
	-Wswitch-enum
	-Wundef
	-Wunreachable-code
	-Wunused
	-Wvariadic-macros
	-Wno-missing-field-initializers
	-Wno-narrowing
	-Wno-old-style-cast
	-Wno-varargs
	-Wstack-protector
	-fcheck-new
	-fsized-deallocation
	-fstack-protector
	-fstrict-overflow
	-fno-omit-frame-pointer
	-Wlarger-than=8192
	-fPIE
	-Werror=vla
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

target_link_options(arc.x PRIVATE
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)


# normal tests
set(TESTS ../tests)

file(GLOB root_testfiles "${TESTS}/*.dat")

file(GLOB_RECURSE subfolder_testfiles "${TESTS}/**/*.dat")

list(APPEND testfiles ${root_testfiles} ${subfolder_testfiles})

foreach(file ${testfiles})
  add_test(
    NAME ${file}
    COMMAND bash -c "${CMAKE_SOURCE_DIR}/../runtest.sh ${file} ./arc.x"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set_tests_properties(${file} PROPERTIES DEPENDS arc.x)
endforeach()

option(ENABLE_BD_TESTS "Big data tests" OFF)

# big data tests
if(ENABLE_BD_TESTS)
	set(BIG_DATA_TESTS ../big_data_tests)

	file(GLOB root_bd_testfiles "${BIG_DATA_TESTS}/*.dat")

	file(GLOB_RECURSE subfolder_bd_testfiles "${BIG_DATA_TESTS}/**/*.dat")

	list(APPEND bd_testfiles ${root_bd_testfiles} ${subfolder_bd_testfiles})

	foreach(file ${bd_testfiles})
	add_test(
		NAME ${file}
		COMMAND bash -c "${CMAKE_SOURCE_DIR}/../runtest.sh ${file} ./arc.x"
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set_tests_properties(${file} PROPERTIES DEPENDS arc.x)
	endforeach()
endif()
