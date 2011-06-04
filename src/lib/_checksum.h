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

#include <synctory.h>


/**
 * Data type for the rolling weak checksum
 */
typedef struct
{
    uint32_t count;     /* number of bytes included in checksum */
    uint32_t s1;        /* part one of the sum */
    uint32_t s2;        /* part two of the sum */
} _synctory_checksum_t;

/**
 * Macro to  initialize the above-defined data type
 */
#define _synctory_checksum_init(sum) { \
    (sum)->count=(sum)->s1=(sum)->s2=0; \
}

/**
 * Macro to shift forward checksum by one byte
 */
#define _synctory_checksum_rotate(sum,out,in) { \
    (sum)->s1 += (unsigned char)(in) - (unsigned char)(out); \
    (sum)->s2 += (sum)->s1 - (sum)->count*((unsigned char)(out)+31); \
}

/**
 * Macro to calculate checksum digest from checksum structure
 */
#define _synctory_checksum_digest(sum) (((sum)->s2 << 16) | ((sum)->s1 & 0xffff))

uint32_t _synctory_weak_checksum(void const *stream, size_t len);
int _synctory_checksum_update(_synctory_checksum_t *checksum, void const *stream, size_t len);
int _synctory_strong_checksum(void const *stream, size_t len, unsigned char *result, synctory_algo_t algo);
int _synctory_rmd160_checksum(void const *stream, size_t len, unsigned char *result);
int _synctory_sha1_checksum(void const *stream, size_t len, unsigned char *result);
int _synctory_strong_checksum_compare(const unsigned char *cs1, const unsigned char *cs2, size_t len);
int _synctory_strong_checksum_size(synctory_algo_t algo);

#endif /* __LIBSYNCTORY_CHECKSUM_H_ */
