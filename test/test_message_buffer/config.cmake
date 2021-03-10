include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_message_buffer.c
	${CMAKE_CURRENT_LIST_DIR}/test_message_buffer_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_message_buffer_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_message_buffer_3.cpp
)
