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
 */

#ifndef __LIBSYNCTORY_VERSION_H
#define __LIBSYNCTORY_VERSION_H

#include <stdint.h>


/*
 * Do not change these definitions!
 */

#define LIBSYNCTORY_CURRENT     0x00
#define LIBSYNCTORY_STABLE      0x22
#define LIBSYNCTORY_ALPHA       0x44
#define LIBSYNCTORY_BETA        0x66
#define LIBSYNCTORY_PRERELEASE  0x88
#define LIBSYNCTORY_RELEASE     0xFF

/*
 * Configure release information here 
 */

#define LIBSYNCTORY_VERSION_MAJOR       1
#define LIBSYNCTORY_VERSION_MINOR       0
#define LIBSYNCTORY_VERSION_PATCH       0
#define LIBSYNCTORY_VERSION_FLAVOUR     LIBSYNCTORY_CURRENT
#define LIBSYNCTORY_VERSION_SUFFX       0

/* 
 * The pattern for the numeric version follows this syntax:
 *
 *   0xAABBCCDDEE
 *
 * AA is the major, BB the minor and CC the patch version.
 * DD represents the flavour and EE the suffix (only applicable
 * to ALPHA, BETA and PRERELEASE flavours; otherwise it will
 * be ignored and should be set to 0).
 * Each of them always represented as two-digit hex number.
 */
#define LIBSYNCTORY_VERSION_NUM \
      (((uint64_t) LIBSYNCTORY_VERSION_MAJOR)   << 32) \
    | (((uint64_t) LIBSYNCTORY_VERSION_MINOR)   << 24) \
    | (((uint64_t) LIBSYNCTORY_VERSION_PATCH)   << 16) \
    | (((uint64_t) LIBSYNCTORY_VERSION_FLAVOUR) << 8) \
    | ((uint64_t)  LIBSYNCTORY_VERSION_SUFFX)

#endif /* __LIBSYNCTORY_VERSION_H */
