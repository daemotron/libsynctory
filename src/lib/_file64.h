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

#ifndef __LIBSYNCTORY_FILE64_H
#define __LIBSYNCTORY_FILE64_H

/*
 * NOTE
 * unistd.h provides the seek whence flags. This include is just for
 * convenience, granting the seek function to work properly even when
 * unistd.h is not explicitly included.
 */
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"

#define _SYNCTORY_FILE64_BUFSIZE 512

/*
 * FIXME
 * 
 * The following piece of code is devoted to the abysmal mystery
 * of glibc (as is this whole module, coming to think of it).
 * 
 * Please, if anyone knows how to get rid of this nauseant,
 * submit a patch!
 */

#if (defined HAVE_STAT_R) && (!defined HAVE_STAT64_R)
typedef struct stat _synctory_file64_stat_t;
#elif (defined HAVE_STAT64_R) && (defined HAVE_LARGEFILE_S)
typedef struct stat64 _synctory_file64_stat_t;
#else
#error "libsynctory only supports 64 bit file pointers!\n"
#endif

#if (OFFT_SIZE == 8)
typedef off_t _synctory_off_t;
#elif (OFFT_SIZE < 8) && (defined OFF64T_SIZE)
typedef off64_t _synctory_off_t;
#else
#error "libsynctory only supports 64 bit file pointers!\n"
#endif

int _synctory_file64_open(const char *path, int oflag, ...);
int _synctory_file64_close(int fd);
_synctory_off_t _synctory_file64_seek(int fd, int64_t offset, int whence);
_synctory_off_t _synctory_file64_bytecopy(int fdsource, int fddest, _synctory_off_t offset, _synctory_off_t bytes);
int _synctory_file64_get_fd(int *flag, int fd, const char *path, char mode);

#endif /* __LIBSYNCTORY_FILE64_H */
