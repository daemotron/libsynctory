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


#ifndef __LIBSYNCTORY_FINGERPRINT_H_
#define __LIBSYNCTORY_FINGERPRINT_H_

#include <stddef.h>
#include <stdint.h>

#include <libsynctory/fingerprint.h>

#include "_fheader.h"
#include "_file64.h"
#include "_checksum.h"

/**
 * @deprecated
 * Byte sequence identifying a fingerprint file
 */
#define SYNCTORY_FINGERPRINT_SEQUENCE 0x53545910U

/**
 * size of the chunks a file will be split into for analysis.
 * chunk sizes between 500 and 1000 bytes ought to be OK.
 *
 * Given the default checksum algorithms create hash chunks
 * of 24 bytes (4 bytes weak, 20 bytes strong), the ratio between
 * originating file size and fingerprint file size is at ~5%.
 */
#define SYNCTORY_CHUNK_BYTES 512U

/**
 * @deprecated
 * size of a synctory fingerprint buffer file header
 */
#define SYNCTORY_FINGERPRINT_HEADER_BYTES 24

/**
 * number of checksums to collect and buffer before writing them to 
 * the fingerprint file
 */
#define SYNCTORY_FINGERPRINT_WRITE_BUFFER 0x400U



/**
 * Data type used for iterative fingerprint file reading
 */
typedef struct
{
	synctory_off_t offset;
	uint16_t chunksize;
	uint8_t algo;
} synctory_fingerprint_iterctx_t;

/**
 * Macro to initialize the above-defined structural data type
 */
#define synctory_fingerprint_iterctx_init(ctx,csize) { \
	(ctx)->offset=0; \
	(ctx)->chunksize=(csize); \
	(ctx)->algo=SYNCTORY_CHECKSUM_DEFAULT; \
}

extern int synctory_fingerprint_fetchheader_fd(int fd, synctory_fheader_t *header);
extern int synctory_fingerprint_fetchheader_fn(const char *fpfile, synctory_fheader_t *header);
extern int synctory_fingerprint_read_iter_fd(int fd, uint32_t *weaksum, unsigned char *strongsum, size_t len, synctory_fingerprint_iterctx_t *ctx);

#endif /* __LIBSYNCTORY_FINGERPRINT_H_ */
