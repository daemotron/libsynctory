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
 * $Id$
 */

#ifndef __LIBSYNCTORY_VERSION_H
#define __LIBSYNCTORY_VERSION_H

#include <stdint.h>

/* Can be one of
	CURRENT
	STABLE
	RELEASE
	ALPHA
	BETA
	PRERELEASE
*/
#define LIBSYNCTORY_VERSION_FLAVOUR "ALPHA"

#define LIBSYNCTORY_VERSION_MAJOR 0
#define LIBSYNCTORY_VERSION_MINOR 1
#define LIBSYNCTORY_VERSION_PATCH 0
#define LIBSYNCTORY_VERSION_SUFFX 1

/* The pattern for the numeric version follows this syntax:

       0xAABBCCDDEE

   AA is the major, BB the minor and CC the patch version,
   each of them always represented as two-digit hex number.
   
   DD represents the flavour and is set thus:
       CURRENT		00
       STABLE		22
       ALPHA		44
       BETA			66
       PRERELEASE	88
       RELEASE		FF
       
	EE is the suffix of the pre-release stages. It only applies
	to ALPHA, BETA and PRERELEASE. In all other cases, it has
	to be set to 00.
*/
#define LIBSYNCTORY_VERSION_NUM 0x0001004401ULL

#endif /* __LIBSYNCTORY_VERSION_H */
