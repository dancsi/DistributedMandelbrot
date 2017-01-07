add_library(kernel INTERFACE)

set(SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/kernel.h)

target_sources(kernel INTERFACE ${SOURCES})

#target_link_libraries(kernel INTERFACE common)