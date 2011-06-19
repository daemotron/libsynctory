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


int hlp_file_bytecopy(const char *source, const char *destination, off_t size, hlp_progress_t *pctx)
{
    off_t rtd = size;
    ssize_t rbytes;
    unsigned char buffer[HLP_CHUNK_SIZE];
    int src, dest;
    uint64_t i = 0, n = 0;
    double k = 0;
    unsigned int dots = 0;
    unsigned int p = 0;
    unsigned int z;
    
    dest = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest < 0)
        return ((errno != 0) ? errno : -1);
    
    src = open(source, O_RDONLY);
    if (dest < 0)
        return ((errno != 0) ? errno : -1);
    
    n = (uint64_t)size / (uint64_t)HLP_CHUNK_SIZE;
    if (0 == ((uint64_t)size % (uint64_t)HLP_CHUNK_SIZE))
        n--;
    
    if (pctx)
        k = ((double)(pctx->width-1)) / ((double)n);
    
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
            p = ((unsigned int)(i*k)) - dots;
            for (z = 0; z < p; z++)
                fprintf(pctx->stream, "%c", pctx->character);
            fflush(pctx->stream);
            dots += p;
        }
    }
    
    memset(buffer, (int)'\0', rtd);
    rbytes = read(src, &buffer[0], rtd);
    if ((off_t)rbytes != rtd)
    {
        fprintf(stderr, "Could not read enough random bytes from %s\n", source);
        close(src);
        close(dest);
        return ((errno != 0) ? errno : -1);
    }
    rbytes = write(dest, &buffer[0], rtd);
    if ((off_t)rbytes != rtd)
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


int hlp_file_randmod(const char *path, unsigned int mod_amount, off_t *positions, unsigned char *orig_chars, unsigned char *mod_chars)
{
    off_t filesize;
    int fd;
    unsigned int i, j, flag;
    void *buf = NULL;
    
    buf = malloc(1);
    if (NULL == buf)
        return errno;
    
    fd = open(path, O_RDWR);
    if (fd < 0)
    {
        free(buf);
        return errno;
    }
    
    filesize = lseek(fd, 0, SEEK_END);
    if (filesize < 0)
    {
        free(buf);
        close(fd);
        return errno;
    }
    
    /* determine positions to change */
    sranddev();
    for (i = 0; i < mod_amount; i++)
    {
        /*
         * Make sure all positions to modify are different from each other.
         * This is necessary to guarantee the requested amount of modifications.
         */
        flag = 1;
        while (flag)
        {
            positions[i] = (rand() % filesize);
            flag = 0;
            for (j = i - 1; j <= i; j--)
                if (positions[j] == positions[i])
                    flag = 1;
        }
        if (1 != pread(fd, buf, 1, positions[i]))
        {
            close(fd);
            free(buf);
            return errno;
        }
        orig_chars[i] = (*(unsigned char*)buf);
        
        /* 
         * Make sure the modified character is different from the original one.
         * This is necessary to guarantee the expected number of modifications.
         */
        mod_chars[i] = orig_chars[i];
        while (mod_chars[i] == orig_chars[i])
            mod_chars[i] = (unsigned char)(rand() % 256);
        if (1 != pwrite(fd,&(mod_chars[i]), 1, positions[i]))
        {
            close(fd);
            free(buf);
            return errno;
        }
    }
    close(fd);
    free(buf);
    return 0;
}


int hlp_file_bincompare(const char* file1, const char* file2)
{
    int rval = 0;
    int fp1, fp2;
    off_t s1, s2, i;
    unsigned char c1 = '\0', c2= '\0';
    
    fp1 = open(file1, O_RDONLY);
    if (fp1 < 0)
        return errno;
    
    fp2 = open(file2, O_RDONLY);
    if (fp2 < 0)
    {
        close(fp1);
        return errno;
    }
    
    s1 = lseek(fp1, 0, SEEK_END);
    s2 = lseek(fp2, 0, SEEK_END);
    lseek(fp1, 0, SEEK_SET);
    lseek(fp2, 0, SEEK_SET);
    
    if ((s1 < 0) || (s2 < 0))
        rval = errno;
    else
    {
        if (s1 == s2)
        {
            for (i = 0; i <= s1; i++)
            {
                c1 = '\0';
                c2 = '\0';
                pread(fp1, &c1, 1, i);
                pread(fp2, &c2, 1, i);
                if (c1 != c2)
                {
                    rval = -1;
                    break;
                }
            }
        }
        else
            rval = -1;
    }
    
    close(fp1);
    close(fp2);
    return rval;
}


void hlp_report_error(int error_no)
{
    if (error_no > 0)
        fprintf(stderr, "Error %d: %s\n", error_no, strerror(error_no));
    else
        fprintf(stderr, "Unknown error.\n");
}
