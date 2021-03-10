include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_fast_mutex.c
	${CMAKE_CURRENT_LIST_DIR}/test_fast_mutex_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_fast_mutex_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_fast_mutex_3.cpp
)
