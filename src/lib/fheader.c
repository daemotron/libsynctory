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


#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <synctory.h>

#include "version.h"

#include "_endianess.h"
#include "_fheader.h"
#include "_file64.h"


/**
 * The functions in this file take care of synctory file header
 * creation and interpretation.
 *
 * The format of a synctory file header is described as follows:
 *
 * Byte  0 -  2    Synctory file identifier
 * Byte  3         Synctory file type
 * Byte  4 - 11    Version number of libsynctory (unsigned, network byte order)
 * Byte 12 - 19    Size of originating file (unsigned, network byte order)
 * Byte 20 - 21    Chunk size used on originating file (unsigned, network byte order)
 * Byte 22         Constant to identify weak and strong checksum combo being used here
 * Byte 23         Zero byte used as header terminator
 */


/**
 * Write a complete file header into a given buffer.
 * The header information is provided in the form of a file header structure.
 */
int
_synctory_fh_setheader_bf(_synctory_fheader_t *header, void *buffer, size_t len)
{
    unsigned char *ptr = (unsigned char *)buffer;
    uint64_t version = _synctory_hton64((uint64_t)_SYNCTORY_VERSION_NUM);
    uint64_t size = _synctory_hton64(header->filesize);
    uint16_t chunksize = _synctory_hton16(header->chunksize);
    uint16_t algo = _synctory_hton16((((uint16_t)header->algo) << 8));
    uint32_t ftype = _synctory_hton32(((uint32_t)_SYNCTORY_FH_IDENTIFIER) | ((uint32_t)header->type));
    int i = 0;
    
    if (len < _SYNCTORY_FH_BYTES)
        return ERANGE;
    
    for (i = 0; i < 4; ++i)
        ptr[i] = *(((unsigned char *)&ftype)+i);
    
    for (i = 0; i < 8; ++i)
        ptr[i+4] = *(((unsigned char *)&version)+i);
    
    for (i = 0; i < 8; ++i)
        ptr[i+12] = *(((unsigned char *)&size)+i);
    
    for (i = 0; i < 2; ++i)
        ptr[i+20] = *(((unsigned char *)&chunksize)+i);
    
    for (i = 0; i < 2; ++i)
        ptr[i+22] = *(((unsigned char *)&algo)+i);
    
    return 0;
}



/**
 * Read a complete file header from a given buffer.
 * The header information is returned by storing it into the provided
 * file header structure.
 */
int
_synctory_fh_getheader_bf(_synctory_fheader_t *header, void *buffer, size_t len)
{
    unsigned char *ptr = (unsigned char *)buffer;
    
    if (len < _SYNCTORY_FH_BYTES)
        return ERANGE;
    
    if (_SYNCTORY_FH_IDENTIFIER != (_synctory_ntoh32(*((uint32_t *)ptr)) & 0xFFFFFF00U))
        return EINVAL;
    
    header->type = (uint8_t)ptr[3];
    header->version = _synctory_ntoh64(*((uint64_t*)&ptr[4]));
    header->filesize = _synctory_ntoh64(*((uint64_t*)&ptr[12]));
    header->chunksize = _synctory_ntoh16(*((uint16_t*)&ptr[20]));
    header->algo = (synctory_algo_t)ptr[22];
    
    return 0;
}


/**
 * Read a complete file header from a given file descriptor.
 * The header information is returned by storing it into the provided
 * file header structure.
 */
int
_synctory_fh_getheader_fd(_synctory_fheader_t *header, int fd)
{
    unsigned char buffer[_SYNCTORY_FH_BYTES];
    ssize_t rbytes;
    
    if (_synctory_file64_seek(fd, 0, SEEK_SET) < 0)
        return errno;
    
    rbytes = read(fd, buffer, _SYNCTORY_FH_BYTES);
    if (rbytes != _SYNCTORY_FH_BYTES)
        return -1;
    
    return _synctory_fh_getheader_bf(header, buffer, _SYNCTORY_FH_BYTES);
}


/**
 * Read a complete file header from a given file name.
 * The header information is returned by storing it into the provided
 * file header structure.
 */
int
_synctory_fh_getheader_fn(_synctory_fheader_t *header, const char *filename)
{
    int fd, rval = 0;
    
    fd = _synctory_file64_open(filename, O_RDONLY);
    if (fd < 0)
        return errno;
    
    rval = _synctory_fh_getheader_fd(header, fd);
    _synctory_file64_close(fd);
    return rval;
}
