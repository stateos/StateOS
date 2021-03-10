include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_event.c
	${CMAKE_CURRENT_LIST_DIR}/test_event_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_event_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_event_3.cpp
)
