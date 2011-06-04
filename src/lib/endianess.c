/*-
 * Copyright (c) 2011 Daemotron <mail@daemotron.net>
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


#include <stdint.h>

#include "_endianess.h"


/**
 * This function implements a simple test for endianess. While it works reasonnably
 * well on little and big endian systems, chances are high it will tell utter rubbish
 * when released on middle or bi-endian systems.
 */
_synctory_endianess_t
__synctory_detect_endianess(void)
{
    uint16_t endiantest = 0x0001;
    return (*(char *)&endiantest == 0x01) ? LITTLEENDIAN : BIGENDIAN;
}


/**
 * Implementation of internal byte swap functions.
 * The implementation is similar to the bswap macros and inline functions used
 * in the FreeBSD kernel,yet they grant usage of the standard int types with
 * well-defined size.
 *
 * The re-implementation became necessary because of the fact that there is
 * no guarantee that htons deals with 16 bit integers under all circumstances
 * (meaning: on all platforms), and inversion for 64 bit integers isn't even
 * part of some standard, so it would be unwise to rely on the libc.
 */
static uint16_t
__synctory_byteswap_16(uint16_t value)
{
    return (value << 8 | value >> 8);
}


static uint32_t
__synctory_byteswap_32(uint32_t value)
{
    return ((value >> 24) | ((value >> 8) & 0xff00) | ((value << 8) & 0xff0000) | (value << 24));
}


static uint64_t
__synctory_byteswap_64(uint64_t value)
{
    return ((value >> 56) | ((value >> 40) & (uint64_t)0xff00) | ((value >> 24) & (uint64_t)0xff0000) | ((value >> 8) & (uint64_t)0xff000000) | ((value << 8) & (uint64_t)0xff00000000) | ((value << 24) & (uint64_t)0xff0000000000) | ((value << 40) & (uint64_t)0xff000000000000) | (value << 56));
}


/**
 * This function implements hton for uint16_t values
 */
uint16_t
_synctory_hton16(uint16_t host16)
{
    if (_SYNCTORY_ENDIANESS == LITTLEENDIAN)
        return __synctory_byteswap_16(host16);
    else
        return host16;
}


/**
 * This function implements hton for uint32_t values
 */
uint32_t
_synctory_hton32(uint32_t host32)
{
    if (_SYNCTORY_ENDIANESS == LITTLEENDIAN)
        return __synctory_byteswap_32(host32);
    else
        return host32;
}


/**
 * This function implements hton for uint64_t values
 */
uint64_t
_synctory_hton64(uint64_t host64)
{
    if (_SYNCTORY_ENDIANESS == LITTLEENDIAN)
        return __synctory_byteswap_64(host64);
    else
        return host64;
}


/**
 * This function implements ntoh for uint16_t values
 */
uint16_t
_synctory_ntoh16(uint16_t net16)
{
    if (_SYNCTORY_ENDIANESS == LITTLEENDIAN)
        return __synctory_byteswap_16(net16);
    else
        return net16;
}


/**
 * This function implements ntoh for uint32_t values
 */
uint32_t
_synctory_ntoh32(uint32_t net32)
{
    if (_SYNCTORY_ENDIANESS == LITTLEENDIAN)
        return __synctory_byteswap_32(net32);
    else
        return net32;
}


/**
 * This function implements ntoh for uint64_t values
 */
uint64_t
_synctory_ntoh64(uint64_t net64)
{
    if (_SYNCTORY_ENDIANESS == LITTLEENDIAN)
        return __synctory_byteswap_64(net64);
    else
        return net64;
}
