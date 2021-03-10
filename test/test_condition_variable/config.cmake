include_guard(GLOBAL)

target_sources(test
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/test_condition_variable.c
	${CMAKE_CURRENT_LIST_DIR}/test_condition_variable_1.c
	${CMAKE_CURRENT_LIST_DIR}/test_condition_variable_2.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_condition_variable_3.cpp
)
