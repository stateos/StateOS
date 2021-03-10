include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_timer.c
	${CMAKE_CURRENT_LIST_DIR}/test_timer_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_timer_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_timer_3.cpp
)
