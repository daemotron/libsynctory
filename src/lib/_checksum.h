/*-
 * Copyright (c) 2010, 2011 Daemotron <mail@daemotron.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#ifndef __LIBSYNCTORY_CHECKSUM_H_
#define __LIBSYNCTORY_CHECKSUM_H_

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <openssl/ripemd.h>


/**
 * @deprecated
 * Define available strong checksum algorithms
 */
#define SYNCTORY_RIPEMD160 	0x10U
#define SYNCTORY_SHA1 		0x20U


/**
 * @deprecated
 * set the default checksum algorithm
 */
#define SYNCTORY_CHECKSUM_DEFAULT SYNCTORY_RIPEMD160

/** 
 * @deprecated
 * size of the strong checksum result in bits.
 * The currently used RIPEMD-160 algorithm produces
 * a 160 bit long checksum 
 */
#define SYNCTORY_STRONG_CHECKSUM_BYTES RIPEMD160_DIGEST_LENGTH
#define SYNCTORY_STRONG_CHECKSUM_BITS SYNCTORY_STRONG_CHECKSUM_BYTES * CHAR_BIT


/**
 * @deprecated
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
extern int synctory_strong_checksum_destbufsize(uint8_t algo);

#endif /* __LIBSYNCTORY_CHECKSUM_H_ */
