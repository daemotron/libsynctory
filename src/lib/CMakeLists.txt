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


include_directories(${libsynctory_SOURCE_DIR}/src/config ${libsynctory_SOURCE_DIR}/src/include ${libsynctory_SOURCE_DIR}/src/lib ${libsynctory_SOURCE_DIR}/src/vendor/tree-1.0 ${libsynctory_BINARY_DIR}/src/config ${libsynctory_BINARY_DIR}/src/include)

# source files that should be compiled into the library have
# to be added to this list
set(
    LIBSYNCTORY_SOURCEFILES
    checksum.c
    diff.c
    endianess.c
    fheader.c
    file64.c
    fingerprint.c
    synth.c
    synctory.c
)

# check whether liblzma can be used
#find_library(LZMA_LIB lzma)
#if(LZMA_LIB)
#	list(APPEND LIBSYNCTORY_LIBRARIES ${LZMA_LIB})
#endif(LZMA_LIB)

# check whether libssl can be used
find_library(SSL_LIB ssl)
if(SSL_LIB)
    list(APPEND LIBSYNCTORY_LIBRARIES ${SSL_LIB})
endif(SSL_LIB)

# build shared library binary
add_library(synctory SHARED ${LIBSYNCTORY_SOURCEFILES})

# link it to other libs
if(LIBSYNCTORY_LIBRARIES)
    target_link_libraries(synctory ${LIBSYNCTORY_LIBRARIES})
endif(LIBSYNCTORY_LIBRARIES)

set_target_properties(synctory PROPERTIES VERSION ${LIBSYNCTORY_LIBVERSION} SOVERSION ${LIBSYNCTORY_LIBVERSION})

install(
    TARGETS synctory
    LIBRARY DESTINATION lib
)