cmake_minimum_required(VERSION 3.5)
project(Mandelbrot)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif()

INCLUDE (CheckIncludeFileCXX)

macro(CHECK_EXPERIMENTAL_INCLUDE INCLUDE)
	CHECK_INCLUDE_FILE_CXX("${INCLUDE}" "HAVE_${INCLUDE}")
	CHECK_INCLUDE_FILE_CXX("experimental/${INCLUDE}" "HAVE_EXPERIMENTAL_${INCLUDE}")
endmacro()

CHECK_EXPERIMENTAL_INCLUDE("optional")
CHECK_EXPERIMENTAL_INCLUDE("string_view")
CHECK_EXPERIMENTAL_INCLUDE("filesystem")
CHECK_INCLUDE_FILE_CXX("experimental/array" "HAVE_EXPERIMENTAL_array")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/common/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/common/config.h)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external/)

add_subdirectory(common/)
add_subdirectory(kernel/)
add_subdirectory(mpi/)
add_subdirectory(queue/)
add_subdirectory(processor/)

set(EXTERNAL_LIBRARIES ${MPI_LIBRARIES} common color kernel mpi_helpers processor queue flags json)
if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
	set(EXTERNAL_LIBRARIES "stdc++fs ${EXTERNAL_LIBRARIES}")
endif()

if(UNIX)
	set(PLATFORM_LINK_LIBRARIES "pthread")
else()
	set(PLATFORM_LINK_LIBRARIES "")
endif()

set(SOURCE_FILES main.cpp)
add_executable(mandelbrot ${SOURCE_FILES})

target_link_libraries(mandelbrot ${EXTERNAL_LIBRARIES} ${PLATFORM_LINK_LIBRARIES})

set_target_properties(mandelbrot PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
set_target_properties(mandelbrot PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
