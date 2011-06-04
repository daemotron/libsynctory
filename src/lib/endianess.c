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
