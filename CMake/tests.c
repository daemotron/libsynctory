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


/**
 * @deprecated
 * 
 * WARNING
 * 
 * This source file is deprecated. Do not use tests from here for the
 * cmake configuration phase!
 */


#ifdef HAVE_GLIBC_STRERROR_R
/* Test for the presence of a glibc style strerror_r function.
 * glibc style strerror_r does not copy the error description
 * into the provided buffer, but returns instead a pointer to
 * the error description
 */
#include <string.h>
#include <errno.h>
int
main () 
{
	char buffer[1024];
	char *string = strerror_r(EACCES, buffer, sizeof(buffer));
	/* this should've returned a string */
	if(!string || !string[0])
		return 99;
	return 0;
}
#endif

#ifdef HAVE_POSIX_STRERROR_R
/* Test for the presence of a POSIX style strerror_r function.
 * POSIX style strerror_r copies the error description into the
 * provided buffer and returns an integer indicating either 
 * success or an error.
 */
#include <string.h>
#include <errno.h>
int
main () {
	char buffer[1024];
	int error = strerror_r(EACCES, buffer, sizeof(buffer));
	/* this should've returned an integer valued zero */
	if(!buffer[0] || error)
		return 99;
	return 0;
}
#endif
