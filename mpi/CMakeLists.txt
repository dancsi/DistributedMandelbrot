add_library(mpi_helpers INTERFACE)

set(SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/message_tags.h
	${CMAKE_CURRENT_SOURCE_DIR}/mpi_helpers.h)

target_sources(mpi_helpers INTERFACE ${SOURCES})
source_group(mpi FILES ${SOURCES})