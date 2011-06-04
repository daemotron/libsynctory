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

#ifndef __LIBSYNCTORY_DIFF_H_
#define __LIBSYNCTORY_DIFF_H_

/**
 * Synctory diff file block type definition for a known chunk.
 */
#define _SYNCTORY_DIFF_BTYPE_CHUNK  0x10U

/**
 * Synctory diff file block type definition for (yet unknown) raw data.
 */
#define _SYNCTORY_DIFF_BTYPE_RAW    0x20U

/**
 * Create a binary diff based on the fingerprint read from the fdfinger
 * file handle, compared to the file content read from the fdsource file
 * handle and stored in the file designated by the fddiff file handle.
 */
int _synctory_diff_create_fd(int fdfinger, int fdsource, int fddiff);

/**
 * Create a binary diff based on the fingerprint read from the file named
 * fingerprint, compared to the file content read from the file named sourcefile
 * and stored in the file named difffile.
 */
int _synctory_diff_create_fn(const char *fingerprint, const char *sourcefile, const char *difffile);

#endif /* __LIBSYNCTORY_DIFF_H_ */
