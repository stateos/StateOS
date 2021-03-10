include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_stream_buffer.c
	${CMAKE_CURRENT_LIST_DIR}/test_stream_buffer_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_stream_buffer_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_stream_buffer_3.cpp
)
