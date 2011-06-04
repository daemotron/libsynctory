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


#ifndef __LIBSYNCTORY_FHEADER_H_
#define __LIBSYNCTORY_FHEADER_H_


#include <stddef.h>
#include <stdint.h>

#include <synctory.h>


/**
 * Length of a synctory file header in bytes.
 * This is also the required size for a buffer to contain
 * a synctory file header
 */
#define _SYNCTORY_FH_BYTES 24U

/**
 * This constant identifies all files created by libsynctory.
 * The last two digits will be filled with the file type by
 * binary ORing with the file type constant.
 */
#define _SYNCTORY_FH_IDENTIFIER  0x53545900U 

/**
 * Synctory file type constants
 */
#define _SYNCTORY_FH_FINGERPRINT  0x10U
#define _SYNCTORY_FH_DIFF         0x20U

/**
 * Generic synctory file header structure
 * All file headers are 24 bytes long and therefore
 * fit into this structural type.
 */
typedef struct
{
    uint8_t type;
    uint64_t version;
    uint64_t filesize;
    uint16_t chunksize;
    synctory_algo_t algo;
} _synctory_fheader_t;


int _synctory_fh_setheader_bf(_synctory_fheader_t *header, void *buffer, size_t len);
int _synctory_fh_getheader_bf(_synctory_fheader_t *header, void *buffer, size_t len);
int _synctory_fh_getheader_fd(_synctory_fheader_t *header, int fd);
int _synctory_fh_getheader_fn(_synctory_fheader_t *header, const char *filename);

#endif /* __LIBSYNCTORY_FHEADER_H_ */
