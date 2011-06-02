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

#include "_fheader.h"
#include "_file64.h"
#include "_checksum.h"

/**
 * @deprecated
 * Byte sequence identifying a fingerprint file
 */
#define SYNCTORY_FINGERPRINT_SEQUENCE 0x53545910U

/**
 * @deprecated
 * size of the chunks a file will be split into for analysis.
 * chunk sizes between 500 and 1000 bytes ought to be OK.
 *
 * Given the default checksum algorithms create hash chunks
 * of 24 bytes (4 bytes weak, 20 bytes strong), the ratio between
 * originating file size and fingerprint file size is at ~5%.
 */
#define SYNCTORY_CHUNK_BYTES 512U

/**
 * @deprecated
 * size of a synctory fingerprint buffer file header
 */
#define SYNCTORY_FINGERPRINT_HEADER_BYTES 24

/**
 * number of checksums to collect and buffer before writing them to 
 * the fingerprint file
 */
#define SYNCTORY_FINGERPRINT_WRITE_BUFFER 0x400U



/**
 * Data type used for iterative fingerprint file reading
 */
typedef struct
{
    synctory_off_t offset;
    uint16_t chunksize;
    uint8_t algo;
} synctory_fingerprint_iterctx_t;

/**
 * Macro to initialize the above-defined structural data type
 */
#define synctory_fingerprint_iterctx_init(ctx,csize) { \
    (ctx)->offset=0; \
    (ctx)->chunksize=(csize); \
    (ctx)->algo=SYNCTORY_CHECKSUM_DEFAULT; \
}

extern int synctory_fingerprint_fetchheader_fd(int fd, synctory_fheader_t *header);
extern int synctory_fingerprint_fetchheader_fn(const char *fpfile, synctory_fheader_t *header);
extern int synctory_fingerprint_read_iter_fd(int fd, uint32_t *weaksum, unsigned char *strongsum, size_t len, synctory_fingerprint_iterctx_t *ctx);

/**
 * Create a fingerprint from the source file descriptor and
 * store it in the file designated by the dest file descriptor.
 */
extern int synctory_fingerprint_create_fd(synctory_ctx_t *ctx, int source, int dest);


/**
 * Create a fingerprint from the file named sourcefile and
 * store it in the file named destfile
 */
extern int synctory_fingerprint_create_fn(synctory_ctx_t *ctx, const char *sourcefile, const char *destfile);

#endif /* __LIBSYNCTORY_FINGERPRINT_H_ */
