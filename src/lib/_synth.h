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


#ifndef __LIBSYNCTORY_SYNTH_H_
#define __LIBSYNCTORY_SYNTH_H_

/**
 * Synthesize recent file from the original file (sourcefile) and
 * the binary difference (diffile) between both. Store the result in
 * the file named destfile.
 */
int _synctory_synth_create_fn(const char *sourcefile, const char *difffile, const char *destfile);

/**
 * Synthesize recent file from the original file (accessed via the fdsource
 * file descriptor) and the binary difference (accessed via the fddiff file
 * descriptor) between both. Store the result in the file designated by the
 * fddest file descriptor.
 */
int _synctory_synth_create_fd(int fdsource, int fddiff, int fddest);

#endif /* __LIBSYNCTORY_SYNTH_H_ */
