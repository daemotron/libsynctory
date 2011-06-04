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
