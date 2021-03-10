include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_alloc.c
	${CMAKE_CURRENT_LIST_DIR}/test_alloc_0.c
	${CMAKE_CURRENT_LIST_DIR}/test_alloc_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_alloc_2.c
	${CMAKE_CURRENT_LIST_DIR}/test_alloc_3.cpp
)
