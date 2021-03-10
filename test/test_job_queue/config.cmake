include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_job_queue.c
	${CMAKE_CURRENT_LIST_DIR}/test_job_queue_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_job_queue_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_job_queue_3.cpp
)
