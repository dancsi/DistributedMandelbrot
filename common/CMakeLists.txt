add_library(common INTERFACE)

set(SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/experimental_cxx_features.h
	${CMAKE_CURRENT_SOURCE_DIR}/job.h
	${CMAKE_CURRENT_SOURCE_DIR}/job_result.h
	${CMAKE_CURRENT_SOURCE_DIR}/logging.h
	${CMAKE_CURRENT_SOURCE_DIR}/vec.h)

target_sources(common INTERFACE ${SOURCES})
source_group(common FILES ${SOURCES})

target_link_libraries(common INTERFACE json gsl spdlog)