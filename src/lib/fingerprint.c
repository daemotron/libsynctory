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

#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include "version.h"
#include "_checksum.h"
#include "_fingerprint.h"
#include "_endianess.h"
#include "_fheader.h"
#include "_file64.h"



/* this is just to make the code more readable:
   number of chunk checksum pairs to buffer * (size of weak checksum + size of strong checksum)
 */
#define __SYNCTORY_FINGERPRINT_DESTBUFSIZE SYNCTORY_FINGERPRINT_WRITE_BUFFER * (sizeof(uint32_t) + SYNCTORY_STRONG_CHECKSUM_BYTES)

extern int
synctory_fingerprint_create_fd(int source, int dest)
{
	unsigned char sourcebuffer[SYNCTORY_CHUNK_BYTES];
	unsigned char header[SYNCTORY_FH_BYTES];
	unsigned char destbuffer[__SYNCTORY_FINGERPRINT_DESTBUFSIZE];
	unsigned char *destptr = &destbuffer[0];
	uint32_t weaksum;
	int i;
	ssize_t rbytes = 0;
	int rval = 0;
	synctory_off_t position;
	synctory_fheader_t fh;
	
	position = synctory_file64_seek(source, 0, SEEK_END);
	if (position < 0)
		return errno;
	
	fh.type = SYNCTORY_FH_FINGERPRINT;
	fh.version = LIBSYNCTORY_VERSION_NUM;
	fh.chunksize = SYNCTORY_CHUNK_BYTES;
	fh.algo = SYNCTORY_CHECKSUM_DEFAULT;
	fh.filesize = (uint64_t)position;
	
	rval = synctory_fh_setheader_bf(&fh, header, SYNCTORY_FH_BYTES);
	if (rval)
		return rval;
	
	position = synctory_file64_seek(dest, 0, SEEK_SET);
	if (position < 0)
		return errno;
	
	/* Write header information into destination file */
	rbytes = write(dest, &header[0], SYNCTORY_FH_BYTES);
	if (rbytes != SYNCTORY_FH_BYTES)
	{
		synctory_file64_close(source);
		synctory_file64_close(dest);
		return -1;
	}
	
	position = synctory_file64_seek(source, 0, SEEK_SET);
	if (position < 0)
		return errno;
	
	/* read chunks from source file until EOF is reached */
	while ((rbytes = read(source, sourcebuffer, SYNCTORY_CHUNK_BYTES)) > 0)
	{
		weaksum = synctory_hton32(synctory_weak_checksum(sourcebuffer, rbytes));
		for (i = 0; i < 4; ++i)
		{
			*destptr = *(((unsigned char *)&weaksum)+i);
			destptr++;
		}
		synctory_strong_checksum(sourcebuffer, rbytes, destptr, SYNCTORY_CHECKSUM_DEFAULT);
		
		if ((unsigned int)(destptr - &destbuffer[0]) >= (__SYNCTORY_FINGERPRINT_DESTBUFSIZE - 1))
		{
			/* buffer needs being flushed */
			rbytes = write(dest, &destbuffer[0], __SYNCTORY_FINGERPRINT_DESTBUFSIZE);
			if (rbytes != __SYNCTORY_FINGERPRINT_DESTBUFSIZE)
			{
				synctory_file64_close(source);
				synctory_file64_close(dest);
				return -1;
			}
			destptr = &destbuffer[0];
		}
		else
		{
			destptr += SYNCTORY_STRONG_CHECKSUM_BYTES;
		}
	}
	
	if ((unsigned int)(destptr - &destbuffer[0]) > 0)
	{
		/* buffer contains data and needs being flushed */
		rbytes = write(dest, &destbuffer[0], (size_t)(destptr - &destbuffer[0]));
		if (rbytes <= 0)
			rval = -1;
	}
	
	return rval;
}


extern int
synctory_fingerprint_create_fn(const char *sourcefile, const char *destfile)
{
	int rval = 0;
	int source, dest;
	
	/* open originating file */
	source = synctory_file64_open(sourcefile, O_RDONLY);
	if (source < 0)
		return errno;
	
	/* open file where the fingerprint should be written to */
	dest = synctory_file64_open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dest < 0)
	{
		synctory_file64_close(source);
		return errno;
	}
	
	rval = synctory_fingerprint_create_fd(source, dest);
	
	synctory_file64_close(source);
	synctory_file64_close(dest);
	return rval;
}


extern int 
synctory_fingerprint_fetchheader_fd(int fd, synctory_fheader_t *header)
{
	return synctory_fh_getheader_fd(header, fd);
}


extern int
synctory_fingerprint_fetchheader_fn(const char *fpfile, synctory_fheader_t *header)
{
	return synctory_fh_getheader_fn(header, fpfile);
}


extern int
synctory_fingerprint_read_iter_fd(int fd, uint32_t *weaksum, unsigned char *strongsum, size_t len, synctory_fingerprint_iterctx_t *ctx)
{
	unsigned char buf[4];
	ssize_t rbytes;
	
	if (0 == ctx->offset)
	{
		synctory_fheader_t header;
		synctory_fingerprint_fetchheader_fd(fd, &header);
		if(SYNCTORY_FH_BYTES != synctory_file64_seek(fd, SYNCTORY_FH_BYTES, SEEK_SET))
			return errno;
		ctx->offset = SYNCTORY_FH_BYTES;
		ctx->algo = header.algo;
	}
	else
		if(0 > synctory_file64_seek(fd, ctx->offset, SEEK_SET))
			return errno;
		
	rbytes = read(fd, buf, 4);
	if (rbytes != 4)
		return -1;
	
	*weaksum = synctory_ntoh32(*((uint32_t *)(&buf[0])));
	rbytes = read(fd, strongsum, len);
	if (rbytes != (ssize_t)len)
		return -1;
	
	ctx->offset += (synctory_strong_checksum_size(ctx->algo) + 4);
	
	return 0;
}
