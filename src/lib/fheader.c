/**
 * Copyright (c) 2010 Jesco Freund.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Backtory nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  $Id$
 */


#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "_endianess.h"
#include "_fheader.h"
#include "_file64.h"
#include "version.h"

/**
 * The functions in this file take care of synctory file header
 * creation and interpretation.
 *
 * The format of a synctory file header is described as follows:
 *
 * Byte  0 -  2    Synctory file identifier
 * Byte  3         Synctory file type
 * Byte  4 - 11    Version number of libsynctory (unsigned, network byte order)
 * Byte 12 - 19    Size of originating file (unsigned, network byte order)
 * Byte 20 - 21    Chunk size used on originating file (unsigned, network byte order)
 * Byte 22         Constant to identify weak and strong checksum combo being used here
 * Byte 23         Zero byte used as header terminator
 */


/**
 * Write a complete file header into a given buffer.
 * The header information is provided in the form of a file header structure.
 */
extern int
synctory_fh_setheader_bf(synctory_fheader_t *header, void *buffer, size_t len)
{
	unsigned char *ptr = (unsigned char *)buffer;
	uint64_t version = synctory_hton64((uint64_t)LIBSYNCTORY_VERSION_NUM);
	uint64_t size = synctory_hton64(header->filesize);
	uint16_t chunksize = synctory_hton16(header->chunksize);
	uint16_t algo = synctory_hton16((((uint16_t)header->algo) << 8));
	uint32_t ftype = synctory_hton32(((uint32_t)SYNCTORY_FH_IDENTIFIER) | ((uint32_t)header->type));
	int i = 0;
	
	if (len < SYNCTORY_FH_BYTES)
		return ERANGE;
	
	for (i = 0; i < 4; ++i)
		ptr[i] = *(((unsigned char *)&ftype)+i);
	
	for (i = 0; i < 8; ++i)
		ptr[i+4] = *(((unsigned char *)&version)+i);
	
	for (i = 0; i < 8; ++i)
		ptr[i+12] = *(((unsigned char *)&size)+i);
	
	for (i = 0; i < 2; ++i)
		ptr[i+20] = *(((unsigned char *)&chunksize)+i);
	
	for (i = 0; i < 2; ++i)
		ptr[i+22] = *(((unsigned char *)&algo)+i);
	
	return 0;
}



/**
 * Read a complete file header from a given buffer.
 * The header information is returned by storing it into the provided
 * file header structure.
 */
extern int
synctory_fh_getheader_bf(synctory_fheader_t *header, void *buffer, size_t len)
{
	unsigned char *ptr = (unsigned char *)buffer;
	
	if (len < SYNCTORY_FH_BYTES)
		return ERANGE;
	
	if (SYNCTORY_FH_IDENTIFIER != (synctory_ntoh32(*((uint32_t *)ptr)) & 0xFFFFFF00U))
		return EINVAL;
	
	header->type = (uint8_t)ptr[3];
	header->version = synctory_ntoh64(*((uint64_t*)&ptr[4]));
	header->filesize = synctory_ntoh64(*((uint64_t*)&ptr[12]));
	header->chunksize = synctory_ntoh16(*((uint16_t*)&ptr[20]));
	header->algo = (uint8_t)ptr[22];
	
	return 0;
}


/**
 * Read a complete file header from a given file descriptor.
 * The header information is returned by storing it into the provided
 * file header structure.
 */
extern int
synctory_fh_getheader_fd(synctory_fheader_t *header, int fd)
{
	unsigned char buffer[SYNCTORY_FH_BYTES];
	ssize_t rbytes;
	
	if (synctory_file64_seek(fd, 0, SEEK_SET) < 0)
		return errno;
	
	rbytes = read(fd, buffer, SYNCTORY_FH_BYTES);
	if (rbytes != SYNCTORY_FH_BYTES)
		return -1;
	
	return synctory_fh_getheader_bf(header, buffer, SYNCTORY_FH_BYTES);
}


/**
 * Read a complete file header from a given file name.
 * The header information is returned by storing it into the provided
 * file header structure.
 */
extern int
synctory_fh_getheader_fn(synctory_fheader_t *header, const char *filename)
{
	int fd, rval = 0;
	
	fd = synctory_file64_open(filename, O_RDONLY);
	if (fd < 0)
		return errno;
	
	rval = synctory_fh_getheader_fd(header, fd);
	synctory_file64_close(fd);
	return rval;
}
