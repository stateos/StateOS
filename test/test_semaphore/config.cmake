include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_semaphore.c
	${CMAKE_CURRENT_LIST_DIR}/test_semaphore_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_semaphore_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_semaphore_3.cpp
)
