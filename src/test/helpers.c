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


#include <sys/param.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"


size_t hlp_path_maxlen(void)
{
    return (size_t)PATH_MAX;
}


int hlp_path_clean(const char *source, char *dest, size_t size)
{
    char auxbuf[PATH_MAX + 1] = { '\0' };
    char *resptr;
    size_t cbytes = (((size-1) < hlp_path_maxlen()) ? (size-1) : hlp_path_maxlen());
    
    resptr = realpath(source, auxbuf);
    
    if (NULL == resptr)
        return errno;
    
    strncpy(dest, auxbuf, cbytes);
    dest[cbytes] = '\0';
    return 0;
}


int hlp_path_join(const char *path1, const char *path2, void *buffer, size_t size)
{
    char *aptr = NULL;
    char *auxbuf = NULL;
    size_t buflen;
    int rval;
    
    buflen = strlen(path1) + strlen(path2) + 2;
    auxbuf = (char *)malloc(buflen);
    if (NULL == auxbuf)
        return errno;
    
    memset(buffer, (int)'\0', size);
    memset(auxbuf, (int)'\0', buflen);
    
    aptr = auxbuf;
    
    strncpy(aptr, path1, strlen(path1));
    aptr += strlen(path1);
    aptr[0] = '/';
    aptr++;
    strncpy(aptr, path2, strlen(path2));
    aptr += strlen(path2);
    aptr[0] = '\0';
    
    rval = hlp_path_clean(auxbuf, buffer, size);
    
    free(auxbuf);
    return rval;
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
                if (!(i % (n/(pctx->width - 1))))
                {
                    fprintf(pctx->stream, "%c", pctx->character);
                    fflush(pctx->stream);
                }
            }
            /* case 2: more progress characters to print than chuncs to copy */
            else
            {
                /* print umph progress characters with every copied chunk */
                for (j = 0; j < ((pctx->width - 1) / n); j++)
                {
                    fprintf(pctx->stream, "%c", pctx->character);
                    fflush(pctx->stream);
                }
            }    
        }
    }
    
    /* print reminaing progress characters (only happens in case 2) */
    if (pctx)
    {
        if (n < (pctx->width - 1))
        {
            for (j = 0; j < ((pctx->width - 1) % n); j++)
            {
                fprintf(pctx->stream, "%c", pctx->character);
                fflush(pctx->stream);
            }
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
    {
        fprintf(pctx->stream, "%c", pctx->character);
        fflush(pctx->stream);
    }
    
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
