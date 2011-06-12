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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"


void hlp_abs_path(const char *source, char *dest, size_t len)
{
    char *ptr = dest;
    size_t fbytes = len - 1;
    size_t cbytes = 0;
    
    if (source[0] != '/')
    {
        char cwd[1024] = { '\0' };
        getcwd(cwd, 1024);
        cbytes = (fbytes < strlen(cwd) ? fbytes: strlen(cwd));
        strncpy(ptr, cwd, cbytes);
        ptr += cbytes;
        fbytes -= cbytes;
        if (fbytes > 0)
        {
            ptr[0] = '/';
            ptr++;
            fbytes--;
            ptr[0] = '\0';
        }
    }
    
    cbytes = (fbytes < (strlen(source) - 1) ? fbytes: (strlen(source) - 1));
    strncpy(ptr, source, cbytes);
    ptr += cbytes;
    fbytes -= cbytes;
    
    if ((fbytes > 0) && (source[strlen(source)-1] != '/'))
    {
        ptr[0] = source[strlen(source)-1];
        ptr += 1;
        fbytes--;
    }
    
    ptr[0] = '\0';
}


int hlp_file_bytecopy(const char *source, const char *destination, size_t size, hlp_progress_t *pctx)
{
    size_t rtd = size;
    ssize_t rbytes;
    unsigned char buffer[HLP_CHUNK_SIZE];
    int src, dest;
    uint64_t i = 0, n = 0, j;
    
    dest = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest < 0)
        return ((errno != 0) ? errno : -1);
    
    src = open(source, O_RDONLY);
    if (dest < 0)
        return ((errno != 0) ? errno : -1);
    
    n = (uint64_t)size / (uint64_t)HLP_CHUNK_SIZE;
    
    while (rtd > HLP_CHUNK_SIZE)
    {
        memset(buffer, (int)'\0', HLP_CHUNK_SIZE);
        rbytes = read(src, &buffer[0], HLP_CHUNK_SIZE);
        if (rbytes != HLP_CHUNK_SIZE)
        {
            fprintf(stderr, "Could not read enough random bytes from %s\n", source);
            close(src);
            close(dest);
            return ((errno != 0) ? errno : -1);
        }
        rbytes = write(dest, &buffer[0], HLP_CHUNK_SIZE);
        if (rbytes != HLP_CHUNK_SIZE)
        {
            fprintf(stderr, "Could not write required number of bytes to %s\n", destination);
            close(src);
            close(dest);
            return ((errno != 0) ? errno : -1);
        }
        rtd -= HLP_CHUNK_SIZE;
        ++i;
        
        /* if progress context has been provided, print progress characters */
        if (pctx)
        {
            /* case 1: more or eq. chunks than progress characters to print */
            if (n >= (pctx->width - 1))
            {
                /* print progress character every umtenth time */
                if (i % (n/(pctx->width - 1)))
                    fprintf(pctx->stream, "%c", pctx->character);
            }
            /* case 2: more progress characters to print than chuncs to copy */
            else
            {
                /* print umph progress characters with every copied chunk */
                for (j = 0; j < ((pctx->width - 1) / n); j++)
                    fprintf(pctx->stream, "%c", pctx->character);
            }    
        }
    }
    
    /* print reminaing progress characters (only happens in case 2) */
    if (pctx)
    {
        if (n < (pctx->width - 1))
        {
            for (j = 0; j < ((pctx->width - 1) % n); j++)
                fprintf(pctx->stream, "%c", pctx->character);
        }
    }
    
    memset(buffer, (int)'\0', rtd);
    rbytes = read(src, &buffer[0], rtd);
    if ((size_t)rbytes != rtd)
    {
        fprintf(stderr, "Could not read enough random bytes from %s\n", source);
        close(src);
        close(dest);
        return ((errno != 0) ? errno : -1);
    }
    rbytes = write(dest, &buffer[0], rtd);
    if ((size_t)rbytes != rtd)
    {
        fprintf(stderr, "Could not write required number of bytes to %s\n", destination);
        close(src);
        close(dest);
        return ((errno != 0) ? errno : -1);
    }
    
    /* print final progress character */
    if (pctx)
        fprintf(pctx->stream, "%c", pctx->character);
    
    close(src);
    close(dest);
    return 0;
}


void hlp_report_error(int error_no)
{
    if (error_no > 0)
        fprintf(stderr, "Error %d: %s\n", error_no, strerror(error_no));
    else
        fprintf(stderr, "Unknown error.\n");
}
