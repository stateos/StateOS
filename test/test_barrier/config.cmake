include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_barrier.c
	${CMAKE_CURRENT_LIST_DIR}/test_barrier_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_barrier_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_barrier_3.cpp
)
