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


#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include "version.h"
#include "_checksum.h"
#include "_fingerprint.h"
#include "_endianess.h"
#include "_fheader.h"
#include "_file64.h"



extern int
synctory_fingerprint_create_fd(synctory_ctx_t *ctx, int source, int dest)
{
    unsigned char header[SYNCTORY_FH_BYTES];
    unsigned char *sourcebuffer = NULL;
    unsigned char *destbuffer;
    unsigned char *destptr = NULL;
    uint32_t weaksum;
    int i;
    ssize_t rbytes = 0;
    int rval = 0;
    synctory_off_t position;
    synctory_fheader_t fh;
    unsigned int destbufsize;
    
    sourcebuffer = (unsigned char *)malloc(ctx->chunk_size);
    if (NULL == sourcebuffer)
        return errno;
    
    destbufsize = SYNCTORY_FINGERPRINT_WRITE_BUFFER * (sizeof(uint32_t) + synctory_strong_checksum_destbufsize((uint8_t)ctx->checksum_algorithm));
    destbuffer = (unsigned char *)malloc(destbufsize);
    if (NULL == destbuffer)
    {
        free(sourcebuffer);
        return errno;
    }
    destptr = &destbuffer[0];
    
    position = synctory_file64_seek(source, 0, SEEK_END);
    if (position < 0)
    {
        free(sourcebuffer);
        free(destbuffer);
        return errno;
    }
    
    fh.type = SYNCTORY_FH_FINGERPRINT;
    fh.version = LIBSYNCTORY_VERSION_NUM;
    fh.chunksize = ctx->chunk_size;
    fh.algo = ctx->checksum_algorithm;
    fh.filesize = (uint64_t)position;
    
    rval = synctory_fh_setheader_bf(&fh, header, SYNCTORY_FH_BYTES);
    if (rval)
    {
        free(sourcebuffer);
        free(destbuffer);
        return rval;
    }
    
    position = synctory_file64_seek(dest, 0, SEEK_SET);
    if (position < 0)
    {
        free(sourcebuffer);
        free(destbuffer);
        return errno;
    }
    
    /* Write header information into destination file */
    rbytes = write(dest, &header[0], SYNCTORY_FH_BYTES);
    if (rbytes != SYNCTORY_FH_BYTES)
    {
        free(sourcebuffer);
        free(destbuffer);
        return -1;
    }
    
    position = synctory_file64_seek(source, 0, SEEK_SET);
    if (position < 0)
    {
        free(sourcebuffer);
        free(destbuffer);
        return errno;
    }
	
    /* read chunks from source file until EOF is reached */
    while ((rbytes = read(source, sourcebuffer, ctx->chunk_size)) > 0)
    {
        weaksum = synctory_hton32(synctory_weak_checksum(sourcebuffer, rbytes));
        for (i = 0; i < 4; ++i)
        {
            *destptr = *(((unsigned char *)&weaksum)+i);
            destptr++;
        }
        synctory_strong_checksum(sourcebuffer, rbytes, destptr, ctx->checksum_algorithm);
        
        if ((unsigned int)(destptr - &destbuffer[0]) >= (destbufsize - 1))
        {
            /* buffer needs being flushed */
            rbytes = write(dest, &destbuffer[0], destbufsize);
            if (rbytes != destbufsize)
            {
                free(sourcebuffer);
                free(destbuffer);
                return -1;
            }
            destptr = &destbuffer[0];
        }
            else
        {
            destptr += synctory_strong_checksum_destbufsize((uint8_t)ctx->checksum_algorithm);
        }
    }
    
    if ((unsigned int)(destptr - &destbuffer[0]) > 0)
    {
        /* buffer contains data and needs being flushed */
        rbytes = write(dest, &destbuffer[0], (size_t)(destptr - &destbuffer[0]));
        if (rbytes <= 0)
        {
            free(sourcebuffer);
            free(destbuffer);
            rval = -1;
        }
    }
    
    free(sourcebuffer);
    free(destbuffer);
    return rval;
}


extern int
synctory_fingerprint_create_fn(synctory_ctx_t *ctx, const char *sourcefile, const char *destfile)
{
    int rval = 0;
    int source, dest;
    
    /* open originating file */
    source = synctory_file64_open(sourcefile, O_RDONLY);
    if (source < 0)
        return errno;
    
    /* open file where the fingerprint should be written to */
    dest = synctory_file64_open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest < 0)
    {
        synctory_file64_close(source);
        return errno;
    }
    
    rval = synctory_fingerprint_create_fd(ctx, source, dest);
    
    synctory_file64_close(source);
    synctory_file64_close(dest);
    return rval;
}


extern int 
synctory_fingerprint_fetchheader_fd(int fd, synctory_fheader_t *header)
{
    return synctory_fh_getheader_fd(header, fd);
}


extern int
synctory_fingerprint_fetchheader_fn(const char *fpfile, synctory_fheader_t *header)
{
    return synctory_fh_getheader_fn(header, fpfile);
}


extern int
synctory_fingerprint_read_iter_fd(int fd, uint32_t *weaksum, unsigned char *strongsum, size_t len, synctory_fingerprint_iterctx_t *ctx)
{
    unsigned char buf[4];
    ssize_t rbytes;
    
    if (0 == ctx->offset)
    {
        synctory_fheader_t header;
        synctory_fingerprint_fetchheader_fd(fd, &header);
        if(SYNCTORY_FH_BYTES != synctory_file64_seek(fd, SYNCTORY_FH_BYTES, SEEK_SET))
                return errno;
        ctx->offset = SYNCTORY_FH_BYTES;
        ctx->algo = header.algo;
    }
    else
        if(0 > synctory_file64_seek(fd, ctx->offset, SEEK_SET))
            return errno;
            
    rbytes = read(fd, buf, 4);
    if (rbytes != 4)
        return -1;
    
    *weaksum = synctory_ntoh32(*((uint32_t *)(&buf[0])));
    rbytes = read(fd, strongsum, len);
    if (rbytes != (ssize_t)len)
        return -1;
    
    ctx->offset += (synctory_strong_checksum_destbufsize(ctx->algo) + 4);
    
    return 0;
}
