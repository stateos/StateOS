include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_once_flag.c
	${CMAKE_CURRENT_LIST_DIR}/test_once_flag_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_once_flag_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_once_flag_3.cpp
)
