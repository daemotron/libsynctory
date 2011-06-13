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


#ifndef __SYNCTORY_TEST_HELPERS_
#define __SYNCTORY_TEST_HELPERS_

#include <stddef.h>
#include <stdio.h>

#define HLP_CHUNK_SIZE  512


typedef struct
{
    unsigned char character;
    unsigned int width;
    FILE *stream;
} hlp_progress_t;


#define hlp_progress_init(ctx) {        \
    (ctx)->character = '\0';            \
    (ctx)->width = 0;                   \
    (ctx)->stream = NULL;               \
}


size_t  hlp_path_maxlen(void);
int     hlp_path_clean(const char *source, char *dest, size_t size);
int     hlp_path_join(const char *path1, const char *path2, void *buffer, size_t size);

int     hlp_file_bytecopy(const char *source, const char *destination, size_t size, hlp_progress_t *pctx);
int     hlp_file_randmod(const char *path, unsigned int mod_amount, off_t *positions, unsigned char *orig_chars, unsigned char *mod_chars);

void    hlp_report_error(int error_no);

#endif /* __SYNCTORY_TEST_HELPERS_ */
