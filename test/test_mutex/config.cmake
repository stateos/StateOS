include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_mutex.c
	${CMAKE_CURRENT_LIST_DIR}/test_mutex_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_mutex_2.c
	${CMAKE_CURRENT_LIST_DIR}/test_mutex_3.c
	${CMAKE_CURRENT_LIST_DIR}/test_mutex_4.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_mutex_5.cpp
)
