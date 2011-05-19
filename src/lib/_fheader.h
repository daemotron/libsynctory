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


#ifndef __LIBSYNCTORY_FHEADER_H_
#define __LIBSYNCTORY_FHEADER_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Length of a synctory file header in bytes.
 * This is also the required size for a buffer to contain
 * a synctory file header
 */
#define SYNCTORY_FH_BYTES 24U

/**
 * This constant identifies all files created by libsynctory.
 * The last two digits will be filled with the file type by
 * binary ORing with the file type constant.
 */
#define SYNCTORY_FH_IDENTIFIER  0x53545900U 

/**
 * Synctory file type constants
 */
#define SYNCTORY_FH_FINGERPRINT  0x10U
#define SYNCTORY_FH_DIFF         0x20U

/**
 * Generic synctory file header structure
 * All file headers are 24 bytes long and therefore
 * fit into this structural type.
 */
typedef struct
{
	uint8_t type;
	uint64_t version;
	uint64_t filesize;
	uint16_t chunksize;
	uint8_t algo;
} synctory_fheader_t;


extern int synctory_fh_setheader_bf(synctory_fheader_t *header, void *buffer, size_t len);
extern int synctory_fh_getheader_bf(synctory_fheader_t *header, void *buffer, size_t len);
extern int synctory_fh_getheader_fd(synctory_fheader_t *header, int fd);
extern int synctory_fh_getheader_fn(synctory_fheader_t *header, const char *filename);

#endif /* __LIBSYNCTORY_FHEADER_H_ */
