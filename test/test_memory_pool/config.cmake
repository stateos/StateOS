include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_memory_pool.c
	${CMAKE_CURRENT_LIST_DIR}/test_memory_pool_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_memory_pool_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_memory_pool_3.cpp
)
