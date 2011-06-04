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

#include <synctory.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "version.h"
#include "default.h"

#include "_file64.h"
#include "_fingerprint.h"
#include "_diff.h"
#include "_synth.h"


extern size_t
synctory_version_bytes(void)
{
    return strlen(PACKAGE_VERSION) + 1;
}


extern void
synctory_version(uint64_t *num, void *buffer, size_t len)
{
    if (NULL != num)
        *num = _SYNCTORY_VERSION_NUM;
    
    if (NULL != buffer)
    {
        char *bf = (char *)buffer;
        size_t maxlen = (strlen(PACKAGE_VERSION) > (len - 1) ? len - 1 : strlen(PACKAGE_VERSION));
        strncpy(bf, PACKAGE_VERSION, maxlen);
        bf[maxlen] = '\0';
    }
}


extern void
synctory_init(synctory_ctx_t *ctx)
{
    ctx->checksum_algorithm = _SYNCTORY_DEFAULT_CHECKSUM;
    ctx->chunk_size = _SYNCTORY_DEFAULT_CHUNKSIZE;
}


extern int
synctory_fingerprint(synctory_ctx_t *ctx, int source_fd, int dest_fd, const char *source_file, const char *dest_file)
{
    /* sfd = source file descriptor, dfd = destination file descriptor */
    int sfd = 0;
    int dfd = 0;
    int flag[2] = {0, 0};
    int rval = 0;
    
    sfd = _synctory_file64_get_fd(&flag[0], source_fd, source_file, 'r');
    dfd = _synctory_file64_get_fd(&flag[1], dest_fd, dest_file, 'w');
    
    rval = _synctory_fingerprint_create_fd(ctx, sfd, dfd);
    
    if (flag[0])
        _synctory_file64_close(sfd);
    if (flag[1])
        _synctory_file64_close(dfd);

    return rval;
}


extern int
synctory_diff(int source_fd, int dest_fd, int fingerprint_fd, const char *source_file, const char *dest_file, const char *fingerprint_file)
{
    /* sfd = source file descriptor, dfd = destination file descriptor */
    int sfd = 0;
    int dfd = 0;
    int ffd = 0;
    int flag[3] = {0, 0, 0};
    int rval = 0;
    
    sfd = _synctory_file64_get_fd(&flag[0], source_fd, source_file, 'r');
    dfd = _synctory_file64_get_fd(&flag[1], dest_fd, dest_file, 'w');
    ffd = _synctory_file64_get_fd(&flag[0], fingerprint_fd, fingerprint_file, 'r');
    
    rval = _synctory_diff_create_fd(ffd, sfd, dfd);
    
    if (flag[0])
        _synctory_file64_close(sfd);
    if (flag[1])
        _synctory_file64_close(dfd);
    if (flag[2])
        _synctory_file64_close(ffd);

    return rval;
}


extern int
synctory_synth(int source_fd, int dest_fd, int diff_fd, const char *source_file, const char *dest_file, const char *diff_file)
{
    /* sfd = source file descriptor, dfd = destination file descriptor */
    int sfd = 0;
    int dfd = 0;
    int ffd = 0;
    int flag[3] = {0, 0, 0};
    int rval = 0;
    
    sfd = _synctory_file64_get_fd(&flag[0], source_fd, source_file, 'r');
    dfd = _synctory_file64_get_fd(&flag[1], dest_fd, dest_file, 'w');
    ffd = _synctory_file64_get_fd(&flag[0], diff_fd, diff_file, 'r');
    
    rval = _synctory_synth_create_fd(sfd, ffd, dfd);
    
    if (flag[0])
        _synctory_file64_close(sfd);
    if (flag[1])
        _synctory_file64_close(dfd);
    if (flag[2])
        _synctory_file64_close(ffd);

    return rval;
}
