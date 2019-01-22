# File for our test suite target creator functions

# Constants
set(TEST_TIMEOUT 30) # seconds
set(COMPILE_LOG_DIR ${CMAKE_BINARY_DIR}/compile-logs)
set(OUTPUT_LOG_DIR ${CMAKE_BINARY_DIR}/test-output)
file(MAKE_DIRECTORY ${COMPILE_LOG_DIR} ${OUTPUT_LOG_DIR})

# There are essentially three cases for how student code is structured.
# Case 1:
# The student provides one or more .cpp files, which we compile into an executable and then run.
# We save the compile output from their code, not ours.
# Case 2:
# The student provides .cpp and/or .h files, which we compile and link with, and then run GTest cases on.
# We save the compile output from building our test cases.
# Case 3:
# The student provides a Makefile that compiles their executable when a certain target is built.
# We save the output from Make in the case, and have to do a make clean first.

# Declare global variables to store 'make grade' command info.

set(MAKE_GRADE_COMMAND_BODY "" CACHE "" INTERNAL FORCE)
set(HAS_ANY_MAKEFILE_PROBLEMS FALSE)


# Function for case 1, where we build the user's executable for them, then execute it
# usage: add_executable_problem(NAME <question name> USER_SOURCE <full path to source files in hw dir> TEST_SOURCE <test source files in current dir, won't be run with Valgrind>)
function(add_executable_problem)
	cmake_parse_arguments(ALS "" "NAME" "USER_SOURCE;TEST_SOURCE" ${ARGN})

	if(NOT ALS_NAME OR NOT ALS_USER_SOURCE OR NOT ALS_TEST_SOURCE)
		message(FATAL_ERROR "Missing arguments!")
	endif()

	# --------------------------------
	# add targets

	# Gracefully handle the case where a source file is missing.
	# It's OK if we get a build error but we need CMake to at least succeed,
	# or the test runner will get in trouble.
	foreach(FILE ${ALS_USER_SOURCE})
		if(NOT EXISTS ${FILE})
			message(WARNING "Source file for problem ${ALS_NAME} could not be found: ${FILE}")
			list(REMOVE_ITEM ALS_USER_SOURCE ${FILE})
		endif()
	endforeach()

	if("${ALS_USER_SOURCE}" STREQUAL "")
		message(WARNING "No user source files exist for ${NAME}")
		return()
	endif()

	# build user code
	add_executable(hw${HW_NUMBER}_${ALS_NAME} ${ALS_USER_SOURCE})

	# build test code
	add_executable(${ALS_NAME}_tests ${ALS_TEST_SOURCE})

	target_link_libraries(${ALS_NAME}_tests gtest gtest_main testing_utils)

	# tell the tests where the executable is through a macro
	string(TOUPPER ${ALS_NAME} NAME_UCASE)
	target_compile_definitions(${ALS_NAME}_tests PUBLIC ${NAME_UCASE}_EXECUTABLE=\"$<TARGET_FILE:hw${HW_NUMBER}_${ALS_NAME}>\")

	# add Google tests
	gtest_add_tests(${ALS_NAME}_tests "" AUTO)

	# make sure that user executable gets built when tests are built
	add_dependencies(${ALS_NAME}_tests hw${HW_NUMBER}_${ALS_NAME})

	# --------------------------------
	# append 'make grade' commands

	# build executable
	set(MAKE_GRADE_COMMAND_BODY ${MAKE_GRADE_COMMAND_BODY} COMMAND ${CMAKE_COMMAND} -E echo "-- Compiling ${ALS_NAME}, output saved to ${COMPILE_LOG_DIR}/${ALS_NAME}.complog" CACHE "" INTERNAL FORCE)
    set(MAKE_GRADE_COMMAND_BODY ${MAKE_GRADE_COMMAND_BODY} COMMAND make hw${HW_NUMBER}_${ALS_NAME} 1>${COMPILE_LOG_DIR}/${ALS_NAME}.complog 2>&1 || true CACHE "" INTERNAL FORCE)

    # build tests
	set(MAKE_GRADE_COMMAND_BODY ${MAKE_GRADE_COMMAND_BODY} COMMAND ${CMAKE_COMMAND} -E echo "-- Compiling ${ALS_NAME} tests" CACHE "" INTERNAL FORCE)
    set(MAKE_GRADE_COMMAND_BODY ${MAKE_GRADE_COMMAND_BODY} COMMAND make ${ALS_NAME}_tests || true CACHE "" INTERNAL FORCE)

endfunction(add_executable_problem)


# Function to create a 'make grade' target using the information gathered
# from calling the add_*_problem() functions.

function(gen_grade_target)

	add_custom_target(grade

		# Setup commands
		COMMAND ${CMAKE_COMMAND} -E echo "-- Cleaning build dir to get a clean record of compilation warnings and errors"
		COMMAND make clean
		COMMAND ${CMAKE_COMMAND} -E echo "-- Compiling testing tools"
		COMMAND make testing_utils
		
		# include commands built by the other functions
		${MAKE_GRADE_COMMAND_BODY}

		# Grade report generation commands
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/Testing                                     # remove any old XML logfiles
		COMMAND ${CMAKE_CTEST_COMMAND} -T Test --no-compress-output -C $<CONFIG> --timeout ${TEST_TIMEOUT} || true   # generate XML logfile
		COMMAND ${CMAKE_COMMAND} -E echo "-- Generating grade report"
		COMMAND python3 ${CMAKE_SOURCE_DIR}/gen_grade_report.py
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

endfunction(gen_grade_target)