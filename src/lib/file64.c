/**
 * Copyright (c) 2010 Jesco Freund.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Backtory nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  $Id$
 */

/*
 * The purpose of this module is to provide a generic interface to file
 * operations such as open, close, write, read, seek with granted
 * 64 bit pointers, allowing to operate on files larger than 2 GiB.
 * 
 * Since these 64 bit wrapper functions are not available on all operating
 * systems, it became necessary to write this library.
 */


#include <sys/types.h>
/*
 * WARNING
 * This is a dirty hack to get lstat() even on systems with glibc.
 * lstat() is only defined there when __USE_XOPEN2K is defined.
 * This is done here, together with a mechanism to unset __USE_XOPEN2K
 * if it hadn't been defined in the first place.
 */
#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#define __BY_XOPEN2K_UNSET
#endif
#include <sys/stat.h>
#ifdef __BY_XOPEN2K_UNSET
#undef __USE_XOPEN2K
#undef __BY_XOPEN2K_UNSET
#endif
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>

#include "config.h"
#include "_file64.h"


int
_synctory_file64_open(const char *path, int oflag, ...)
{
    va_list vargptr;
    mode_t mode;

#ifdef HAVE_LARGEFILE_S
    oflag |= O_LARGEFILE;
#endif

    if ((O_CREAT & oflag) == O_CREAT)
    {
        va_start(vargptr, oflag);
#ifdef __FreeBSD__
        mode = va_arg(vargptr, int);
#else
        mode = va_arg(vargptr, mode_t);
#endif
        va_end(vargptr);
#if (OFFT_SIZE == 8) || ((OFFT_SIZE == 4) && (!defined HAVE_OPEN64_F) && (defined HAVE_LARGEFILE_S))
        return open(path, oflag, mode);
#elif (OFFT_SIZE == 4) && (defined HAVE_OPEN64_F)
        return open64(path, oflag, mode);
#else
#error "libsynctory only supports 64 bit file pointers!\n"
#endif
    }
    else
    {
#if (OFFT_SIZE == 8) || ((OFFT_SIZE == 4) && (!defined HAVE_OPEN64_F) && (defined HAVE_LARGEFILE_S))
        return open(path, oflag);
#elif (OFFT_SIZE == 4) && (defined HAVE_OPEN64_F)
        return open64(path, oflag);
#else
#error "libsynctory only supports 64 bit file pointers!\n"
#endif
    }
    return 0;
}


int 
_synctory_file64_get_fd(int *flag, int fd, const char *path, char mode)
{
    if (fd > 0)
    {
        *flag = 0;
        return fd;
    }
    else
    {
        if (NULL == path)
            return -1;
        else
        {
            *flag = 1;
            if ('r' == mode)
                return _synctory_file64_open(path, O_RDONLY);
            else if ('w' == mode)
                return _synctory_file64_open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            else
                return -1;
        }
    }
}

int
_synctory_file64_close(int fd)
{
    return close(fd);
}


_synctory_off_t
_synctory_file64_seek(int fd, int64_t offset, int whence)
{
#if (OFFT_SIZE == 8) || ((OFFT_SIZE == 4) && (!defined HAVE_LSEEK64_F) && (defined HAVE_LARGEFILE_S))
    return (_synctory_off_t)lseek(fd, (off_t)offset, whence);
#elif (OFFT_SIZE == 4) && (defined HAVE_LSEEK64_F) && (defined OFF64T_SIZE)
    return (_synctory_off_t)lseek64(fd, (off64_t)offset, whence);
#else
#error "libsynctory only supports 64 bit file pointers!\n"
#endif
    return 0;
}


int
_synctory_file64_lstat(const char *file, _synctory_file64_stat_t *buf)
{
#if (defined HAVE_LSTAT64_F) && (defined HAVE_STAT64_R)
    return lstat64(file, buf);
#else
    return lstat(file, buf);
#endif
}


_synctory_off_t
_synctory_file64_bytecopy(int fdsource, int fddest, _synctory_off_t offset, _synctory_off_t bytes)
{
    int rval = 0;
    unsigned char buffer[_SYNCTORY_FILE64_BUFSIZE];
    _synctory_off_t position;
    
    /* seek to the start position of the source */
    if (offset != _synctory_file64_seek(fdsource, offset, SEEK_SET))
        return errno;
    
    /* transfer raw bytes to diff file */
    for (position = 0; position < (bytes / _SYNCTORY_FILE64_BUFSIZE); position++)
    {
        read(fdsource, buffer, _SYNCTORY_FILE64_BUFSIZE);
        rval += write(fddest, buffer, _SYNCTORY_FILE64_BUFSIZE);
    }
    if ((bytes % _SYNCTORY_FILE64_BUFSIZE) != 0)
    {
        read(fdsource, buffer, bytes % _SYNCTORY_FILE64_BUFSIZE);
        rval += write(fddest, buffer, bytes % _SYNCTORY_FILE64_BUFSIZE);
    }
    
    return rval;
}
