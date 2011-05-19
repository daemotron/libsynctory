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

#ifndef __LIBSYNCTORY_CHECKSUM_H_
#define __LIBSYNCTORY_CHECKSUM_H_

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <openssl/ripemd.h>


/**
 * Define available strong checksum algorithms
 */
#define SYNCTORY_RIPEMD160 	0x10U
#define SYNCTORY_SHA1 		0x20U


/**
 * set the default checksum algorithm
 */
#define SYNCTORY_CHECKSUM_DEFAULT SYNCTORY_RIPEMD160

/** 
 * size of the strong checksum result in bits.
 * The currently used RIPEMD-160 algorithm produces
 * a 160 bit long checksum 
 */
#define SYNCTORY_STRONG_CHECKSUM_BYTES RIPEMD160_DIGEST_LENGTH
#define SYNCTORY_STRONG_CHECKSUM_BITS SYNCTORY_STRONG_CHECKSUM_BYTES * CHAR_BIT


/**
 * Dummy macro to return correct size of a strong checksum
 */
#define synctory_strong_checksum_size(algo) SYNCTORY_STRONG_CHECKSUM_BYTES

/**
 * Data type for the rolling weak checksum
 */
typedef struct
{
	uint32_t count;		/* number of bytes included in checksum */
	uint32_t s1;		/* part one of the sum */
	uint32_t s2;		/* part two of the sum */
} synctory_checksum_t;

/**
 * Macro to  initialize the above-defined data type
 */
#define synctory_checksum_init(sum) { \
	(sum)->count=(sum)->s1=(sum)->s2=0; \
}

/**
 * Macro to shift forward checksum by one byte
 */
#define synctory_checksum_rotate(sum,out,in) { \
	(sum)->s1 += (unsigned char)(in) - (unsigned char)(out); \
	(sum)->s2 += (sum)->s1 - (sum)->count*((unsigned char)(out)+31); \
}

/**
 * Macro to calculate checksum digest from checksum structure
 */
#define synctory_checksum_digest(sum) (((sum)->s2 << 16) | ((sum)->s1 & 0xffff))

extern uint32_t synctory_weak_checksum(void const *stream, size_t len);
extern int synctory_checksum_update(synctory_checksum_t *checksum, void const *stream, size_t len);
extern int synctory_strong_checksum(void const *stream, size_t len, unsigned char *result, uint8_t algo);
extern int synctory_rmd160_checksum(void const *stream, size_t len, unsigned char *result);
extern int synctory_sha1_checksum(void const *stream, size_t len, unsigned char *result);
extern int synctory_strong_checksum_compare(const unsigned char *cs1, const unsigned char *cs2, size_t len);

#endif /* __LIBSYNCTORY_CHECKSUM_H_ */
