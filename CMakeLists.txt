cmake_minimum_required(VERSION 3.7)
project(Mandelbrot)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(external/)

add_subdirectory(common/)
add_subdirectory(kernel/)
add_subdirectory(mpi/)
add_subdirectory(queue/)
add_subdirectory(processor/)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif()

set(EXTERNAL_LIBRARIES ${MPI_LIBRARIES} common color kernel processor queue flags json)

set(SOURCE_FILES main.cpp)
add_executable(mandelbrot ${SOURCE_FILES})

target_link_libraries(mandelbrot ${EXTERNAL_LIBRARIES})

set_target_properties(mandelbrot PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
set_target_properties(mandelbrot PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
