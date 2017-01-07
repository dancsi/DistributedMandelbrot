add_library(concurrentqueue INTERFACE )
target_include_directories(concurrentqueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/concurrentqueue.h)

#target_sources(concurrentqueue INTERFACE ${SOURCE})