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
#include <unistd.h>

#include "_diff.h"
#include "_endianess.h"
#include "_fheader.h"
#include "_file64.h"
#include "_synth.h"


extern int
synctory_synth_create_fd(int fdsource, int fddiff, int fddest)
{
	int rval = 0;
	synctory_fheader_t header;
	uint8_t type;
	uint64_t index;
	unsigned char ibuf[9];
	synctory_off_t offset;
	ssize_t rbytes;
	
	/* try to read header from diff file */
	if ((rval = synctory_fh_getheader_fd(&header, fddiff)) != 0)
		return rval;
	
	/* position diff file pointer at beginning of data section */
	if ((offset = synctory_file64_seek(fddiff, SYNCTORY_FH_BYTES, SEEK_SET)) != SYNCTORY_FH_BYTES)
		return errno;
	
	while ((rbytes = read(fddiff, ibuf, 9)) == 9)
	{
		type = *((uint8_t *)&ibuf[0]);
		index = synctory_ntoh64(*((uint64_t *)&ibuf[1]));
		
		switch (type)
		{
			case SYNCTORY_DIFF_BTYPE_CHUNK:
				synctory_file64_bytecopy(fdsource, fddest, index * header.chunksize, header.chunksize);
				break;
				
			case SYNCTORY_DIFF_BTYPE_RAW:
				synctory_file64_bytecopy(fddiff, fddest, synctory_file64_seek(fddiff, 0, SEEK_CUR), index);
				break;
				
			default:
				return -1;
		}
	}
	
	return rval;
}


extern int
synctory_synth_create_fn(const char *sourcefile, const char *difffile, const char *destfile)
{
	int rval = 0;
	int fdsource, fddiff, fddest;
	
	/* open read-only file descriptor for source file */
	fdsource = synctory_file64_open(sourcefile, O_RDONLY);
	if (fdsource < 0)
		return errno;
	
	/* open read-only file descriptor for diff file */
	fddiff = synctory_file64_open(difffile, O_RDONLY);
	if (fddiff < 0)
		return errno;
	
	/* open write-only file descriptor for diff file */
	fddest = synctory_file64_open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fddest < 0)
		return errno;
	
	rval = synctory_synth_create_fd(fdsource, fddiff, fddest);
	
	synctory_file64_close(fdsource);
	synctory_file64_close(fddiff);
	synctory_file64_close(fddest);
	
	return rval;
}
