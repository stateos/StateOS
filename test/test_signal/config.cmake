include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_signal.c
	${CMAKE_CURRENT_LIST_DIR}/test_signal_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_signal_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_signal_3.cpp
)
