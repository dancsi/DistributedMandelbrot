add_library(color INTERFACE)
target_include_directories(color INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/color.hpp)
