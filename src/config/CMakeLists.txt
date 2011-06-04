#-
# Copyright (c) 2011 Daemotron <mail@daemotron.net>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


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
