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
int _synctory_diff_create_fast(int fdfinger, int fdsource, int fddiff);

/**
 * Create binary diff by using a low memory profile (slow!)
 */
int _synctory_diff_create_fd(int fdfinger, int fdsource, int fddiff);

/**
 * Create a binary diff based on the fingerprint read from the file named
 * fingerprint, compared to the file content read from the file named sourcefile
 * and stored in the file named difffile.
 */
int _synctory_diff_create_fn(const char *fingerprint, const char *sourcefile, const char *difffile);

#endif /* __LIBSYNCTORY_DIFF_H_ */
