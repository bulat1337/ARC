cmake_minimum_required(VERSION 3.14)

project(cache)

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/unit_tests/arc)
add_subdirectory(${CMAKE_SOURCE_DIR}/unit_tests/perfect)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(ENABLE_LOGGING "Enable logging" OFF)
option(BUILD_ARC "Build the arc executable" ON)
option(BUILD_PERFECT "Build the perfect executable" ON)
option(ENABLE_ARC_TESTS "Enable tests for arc executable" ON)
option(ENABLE_PERFECT_TESTS "Enable tests for perfect executable" ON)
option(ENABLE_BD_TESTS "Big data tests for arc.x and perfect.x" OFF)
set(ENABLE_BD_TESTS ${ENABLE_BD_TESTS} CACHE BOOL "Enable Big Data Tests" FORCE)

option(ENABLE_ARC_BD_TESTS "Big data tests for arc.x" OFF)
option(ENABLE_PERFECT_BD_TESTS "Big data tests for perfect.x" OFF)
option(DEBUG "Debug version" OFF)

set(RELEASE_COMPILE_OPTIONS
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

set(DEBUG_COMPILE_OPTIONS
	-D_DEBUG
	-ggdb3
	-O0
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
	-Wformat-security
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

# ----- ARC executable -----
if(BUILD_ARC)
	add_executable(arc.x ./arc_main.cpp)

	target_include_directories(arc.x PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/utils/include)

	if(ENABLE_LOGGING)
		target_compile_definitions(arc.x PRIVATE ENABLE_LOGGING)
	endif()

	if(DEBUG)
		target_compile_options(arc.x PRIVATE ${DEBUG_COMPILE_OPTIONS})

		target_link_options(arc.x PRIVATE
			-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
		)
	else()
		target_compile_options(arc.x PRIVATE ${RELEASE_COMPILE_OPTIONS})
	endif()
endif()

# ----- PERFECT executable -----
if(BUILD_PERFECT)
	add_executable(perfect.x ./perfect_main.cpp)
	target_include_directories(perfect.x PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/utils/include)

	if(ENABLE_LOGGING)
		target_compile_definitions(perfect.x PRIVATE ENABLE_LOGGING)
	endif()

	if(DEBUG)
		target_compile_options(perfect.x PRIVATE ${DEBUG_COMPILE_OPTIONS})

		target_link_options(perfect.x PRIVATE
			-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
		)
	else()
		target_compile_options(perfect.x PRIVATE ${RELEASE_COMPILE_OPTIONS})
	endif()
endif()



