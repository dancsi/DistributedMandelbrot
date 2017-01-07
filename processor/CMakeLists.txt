add_library(processor INTERFACE)

set(SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/processor_base.h
	${CMAKE_CURRENT_SOURCE_DIR}/mpi_server.h
	${CMAKE_CURRENT_SOURCE_DIR}/worker_st.h
	${CMAKE_CURRENT_SOURCE_DIR}/worker_mt.h)

target_sources(processor INTERFACE ${SOURCES})

#target_include_directories(common INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")