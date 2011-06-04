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


#ifndef __LIBSYNCTORY_FINGERPRINT_H_
#define __LIBSYNCTORY_FINGERPRINT_H_


#include <stddef.h>
#include <stdint.h>

#include <synctory.h>

#include "default.h"

#include "_fheader.h"
#include "_file64.h"
#include "_checksum.h"


/**
 * number of checksums to collect and buffer before writing them to 
 * the fingerprint file
 */
#define _SYNCTORY_FINGERPRINT_WRITE_BUFFER 0x400U


/**
 * Data type used for iterative fingerprint file reading
 */
typedef struct
{
    _synctory_off_t offset;
    uint16_t chunksize;
    synctory_algo_t algo;
} _synctory_fingerprint_iterctx_t;

/**
 * Macro to initialize the above-defined structural data type
 */
#define _synctory_fingerprint_iterctx_init(ctx,csize,calgo) { \
    (ctx)->offset=0; \
    (ctx)->chunksize=(csize); \
    (ctx)->algo=(calgo); \
}

int _synctory_fingerprint_fetchheader_fd(int fd, _synctory_fheader_t *header);
int _synctory_fingerprint_fetchheader_fn(const char *fpfile, _synctory_fheader_t *header);
int _synctory_fingerprint_read_iter_fd(int fd, uint32_t *weaksum, unsigned char *strongsum, size_t len, _synctory_fingerprint_iterctx_t *ctx);

/**
 * Create a fingerprint from the source file descriptor and
 * store it in the file designated by the dest file descriptor.
 */
int _synctory_fingerprint_create_fd(synctory_ctx_t *ctx, int source, int dest);


/**
 * Create a fingerprint from the file named sourcefile and
 * store it in the file named destfile
 */
int _synctory_fingerprint_create_fn(synctory_ctx_t *ctx, const char *sourcefile, const char *destfile);

#endif /* __LIBSYNCTORY_FINGERPRINT_H_ */
