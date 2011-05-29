include_directories(${libsynctory_SOURCE_DIR}/src/config ${libsynctory_SOURCE_DIR}/src/include ${libsynctory_SOURCE_DIR}/src/lib ${libsynctory_BINARY_DIR}/src/config ${libsynctory_BINARY_DIR}/src/include)

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