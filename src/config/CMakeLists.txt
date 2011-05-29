include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckStructExists)

check_include_files(openssl/ssl.h HAVE_OPENSSL_H)
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

# Write result of tests into config.h
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
