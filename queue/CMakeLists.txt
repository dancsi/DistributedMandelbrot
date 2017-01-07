add_library(queue INTERFACE)

set(SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/queue_base.h
	${CMAKE_CURRENT_SOURCE_DIR}/queue_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/queue_mpi.h
	${CMAKE_CURRENT_SOURCE_DIR}/result_collector.h)

target_sources(queue INTERFACE ${SOURCES})

#target_include_directories(common INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")