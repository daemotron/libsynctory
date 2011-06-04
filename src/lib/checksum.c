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


#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <openssl/err.h>

#include <synctory.h>

#include "_checksum.h"

/**
 * This function implements the rolling checksum function as described by
 * [http://rsync.samba.org/tech_report/node3.html]. This checksum function
 * is not very collision-safe, but very cheap in calculation (compared to
 * more complex checksums like e. g. SHA-256), particularly when used in
 * sequential mode.
 *
 * ATTENTION:
 * The behaviour of this function is only granted for byte sequences with 
 * a maximum length of 5803 bytes. Using longer numeric types (like e. g. 
 * uint64_t) would extend this range, but also entail a loss in performance
 * and increase the memory footprint.
 */
uint32_t
_synctory_weak_checksum(void const *stream, size_t len)
{
    int i;
    uint32_t a = 0, b = 0;
    unsigned char const *message = (unsigned char const *)stream;
    
    /* work through the first n 4-byte blocks - this is cheaper than doing it byte by byte...
        int cast is necessary for the difference can become a negative number,
        otherwise buffers with a length < 4 bytes will provoke a segmentation fault
    */
    for (i = 0; i < ((int)len - 4); i += 4)
    {
        b += 4 * (a + message[i]) + 3  * message[i+1] + 2 * message[i+2] + message[i+3] + 310;
        a += (message[i] + message[i+1] + message[i+2] + message[i+3] + 124); 
    }
    
    /* take care of the remaining bytes that do not build a 4-byte block */
    for (;i < (int)len; i++)
    {
        a += (message[i] + 31);
        b += a;
    }
    
    /* b makes the first 2 hex digits, a the last 2 ones */
    return (a & 0xffff) + (b << 0x10);
}


/**
 * Alternative implementation of above-described checksum.
 * This implementation leaves a and b separated in the result, so the checksum can
 * easily be rotated. For a full buffer checksum, this implementation however is 
 * slightly more expensive than the one-shot implementation above.
 */

/*
 * NOTE
 * These macros render the implementation below more legible; they do not
 * have any other purpose.
 */
#define SYNCTORY_CHECKSUM_STEP1(buffer,i)  	{a += buffer[i]; b += a;}
#define SYNCTORY_CHECKSUM_STEP2(buffer,i)  	SYNCTORY_CHECKSUM_STEP1(buffer,i); SYNCTORY_CHECKSUM_STEP1(buffer,i+1);
#define SYNCTORY_CHECKSUM_STEP4(buffer,i)  	SYNCTORY_CHECKSUM_STEP2(buffer,i); SYNCTORY_CHECKSUM_STEP2(buffer,i+2);
#define SYNCTORY_CHECKSUM_STEP8(buffer,i)  	SYNCTORY_CHECKSUM_STEP4(buffer,i); SYNCTORY_CHECKSUM_STEP4(buffer,i+4);
#define SYNCTORY_CHECKSUM_STEP16(buffer)   	SYNCTORY_CHECKSUM_STEP8(buffer,0); SYNCTORY_CHECKSUM_STEP8(buffer,8);
#define SYNCTORY_CHECKSUM_OFFS16(off)  		{a += 16*off; b += 136*off;}

int
_synctory_checksum_update(_synctory_checksum_t *checksum, void const *stream, size_t len)
{
    uint32_t a = checksum->s1;
    uint32_t b = checksum->s2;
    unsigned char const *message = (unsigned char const *)stream;
    
    checksum->count += len;
    
    /* work through the first n 16-byte blocks - this is cheaper than doing it byte by byte... */
    while (len >= 16)
    {
        SYNCTORY_CHECKSUM_STEP16(message);
        SYNCTORY_CHECKSUM_OFFS16(31);
        message += 16;
        len -= 16;
    }
    
    /* take care of the remaining bytes that do not build a 16-byte block */
    while (len != 0)
    {
        a += (*message++ + 31);
        b += a;
        len--;
    }
    checksum->s1 = a;
    checksum->s2 = b;
    
    return 0;
}


int
_synctory_rmd160_checksum(void const *stream, size_t len, unsigned char *result)
{
    if (NULL == RIPEMD160((unsigned char const *)stream, len, result))
        return ERR_get_error();
    return 0;
}


int
_synctory_sha1_checksum(void const *stream, size_t len, unsigned char *result)
{
    if (NULL == SHA1((unsigned char const *)stream, len, result))
        return ERR_get_error();
    return 0;
}


int 
_synctory_strong_checksum_size(synctory_algo_t algo)
{
    switch (algo)
    {
        case synctory_algo_rmd160:
            return RIPEMD160_DIGEST_LENGTH;
            break;
        case synctory_algo_sha1:
            return SHA_DIGEST_LENGTH;
        default:
            return -1;
    }
}


/**
 * This function implements a strong checksum function. Currently the OpenSSL 
 * implementation of the RIPEMD-160 algorith is being used as the default
 * to create such a strong checksum.
 *
 * Since the strong checksum result is larger than the weak checksum result, 
 * a buffer with a length of SYNCTORY_STRONG_CHECKSUM_BYTES has to be
 * provided to this function so it can save the result to this buffer.
 */
int
_synctory_strong_checksum(void const *stream, size_t len, unsigned char *result, synctory_algo_t algo)
{
    switch (algo)
    {
        case synctory_algo_rmd160:
            return _synctory_rmd160_checksum(stream, len, result);
            break;
        case synctory_algo_sha1:
            return _synctory_sha1_checksum(stream, len, result);
            break;
        default:
            return -1;
    }
}


/**
 * Compare two strong checksums
 */
int
_synctory_strong_checksum_compare(const unsigned char *cs1, const unsigned char *cs2, size_t len)
{
    int i;
    for (i = 0; i < (int)len; ++i)
    {
        if (cs1[i] > cs2[i])
            return 1;
        if (cs1[i] < cs2[i])
            return -1;
    }
    return 0;
}
