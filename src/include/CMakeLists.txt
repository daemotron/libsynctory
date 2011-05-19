include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckStructExists)

check_include_files(openssl/ssl.h HAVE_OPENSSL_H)
check_include_files(lzma.h HAVE_LZMA_H)
check_include_files(pthread.h HAVE_PTHREAD_H)

set(CMAKE_EXTA_INCLUDE_FILES sys/types.h)
check_type_size("off_t" OFFT_SIZE)
check_type_size("off64_t" OFF64T_SIZE)
set(CMAKE_EXTA_INCLUDE_FILES)

set(CMAKE_EXTA_INCLUDE_FILES unistd.h)
check_type_size("ssize_t" SSIZET_SIZE)
check_type_size("size_t" SIZET_SIZE)
set(CMAKE_EXTA_INCLUDE_FILES)

check_struct_exists("struct stat" "sys/types.h;sys/stat.h" HAVE_STAT_R)
check_struct_exists("struct stat64" "sys/types.h;sys/stat.h" HAVE_STAT64_R)

check_symbol_exists(O_LARGEFILE "bits/fcntl.h" HAVE_LARGEFILE_S)
check_function_exists(open64 HAVE_OPEN64_F)
check_function_exists(lseek64 HAVE_LSEEK64_F)
check_function_exists(lstat64 HAVE_LSTAT64_F)

# FIXME
# The following macro doesn't work properly since the tests
# source file doesn't compile properly using -std=c99.

# Test Macro
# 
# This macro compiles CMake/tests.c with the appropriate
# -DTESTNAME flag for the requested test. To implement a
# test, just add a section in CMake/tests.c encased by a
# #define HAVE_<TESTNAME> preprocessor directive. This section
# necessarily has to contain a main function returning
# 0 for a positive test result or some value >0 for a failed
# test.
#
# WARNING
# this is extremely ugly; I hope to replace this by something
# more clean and bearable...
macro(LIBSYNCTORY_INTERNAL_TEST_RUN LIBSYNCTORY_TEST)
	if("${LIBSYNCTORY_TEST}_COMPILE" MATCHES "^${LIBSYNCTORY_TEST}_COMPILE$")
		set(MACRO_CHECK_FUNCTION_DEFINITIONS "-D${LIBSYNCTORY_TEST} ${CMAKE_REQUIRED_FLAGS}")

		if(CMAKE_REQUIRED_LIBRARIES)
			set(LIBSYNCTORY_TEST_ADD_LIBRARIES "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
		endif(CMAKE_REQUIRED_LIBRARIES)
		
		message(STATUS "Performing libsynctory Test ${LIBSYNCTORY_TEST}")
		try_run(
			${LIBSYNCTORY_TEST}
			${LIBSYNCTORY_TEST}_COMPILE
			${CMAKE_BINARY_DIR}
			${libsynctory_SOURCE_DIR}/CMake/tests.c
			CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FUNCTION_DEFINITIONS}
			"${LIBSYNCTORY_TEST_ADD_LIBRARIES}"
			OUTPUT_VARIABLE OUTPUT
		)
		
		if(${LIBSYNCTORY_TEST}_COMPILE AND NOT ${LIBSYNCTORY_TEST})
			set(${LIBSYNCTORY_TEST} 1 CACHE INTERNAL "libsynctory test ${FUNCTION}")
			message(STATUS "Performing libsynctory Test ${LIBSYNCTORY_TEST} - Success")
		else(${LIBSYNCTORY_TEST}_COMPILE AND NOT ${LIBSYNCTORY_TEST})
			message(STATUS "Performing libsynctory Test ${LIBSYNCTORY_TEST} - Failed")
			set(${LIBSYNCTORY_TEST} "" CACHE INTERNAL "libsynctory test ${FUNCTION}")
			file(
				APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log"
				"Performing libsynctory Test ${LIBSYNCTORY_TEST} failed with the following output:\n"
				"${OUTPUT}"
			)
			if(${LIBSYNCTORY_TEST}_COMPILE)
				file(
					APPEND
					"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log"
					"There was a problem running this test\n"
				)
			endif(${LIBSYNCTORY_TEST}_COMPILE)
			file(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log" "\n\n")
		endif(${LIBSYNCTORY_TEST}_COMPILE AND NOT ${LIBSYNCTORY_TEST})
	endif("${LIBSYNCTORY_TEST}_COMPILE" MATCHES "^${LIBSYNCTORY_TEST}_COMPILE$")
endmacro(LIBSYNCTORY_INTERNAL_TEST_RUN)

# Run test macro for several tests
foreach(LIBSYNCTORY_TEST
	HAVE_GLIBC_STRERROR_R
	HAVE_POSIX_STRERROR_R
	)
	libsynctory_internal_test_run(${LIBSYNCTORY_TEST})
endforeach(LIBSYNCTORY_TEST)

# Write result of tests into config.h
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# install header files
install(
	FILES synctory.h
	DESTINATION include
)

# install libsynctory directory
install(
	DIRECTORY libsynctory
	DESTINATION include
	FILES_MATCHING PATTERN "*.txt" EXCLUDE
)

add_subdirectory(libsynctory)