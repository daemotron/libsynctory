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
#include <unistd.h>

#include "_diff.h"
#include "_endianess.h"
#include "_fheader.h"
#include "_file64.h"
#include "_synth.h"


int
_synctory_synth_create_fd(int fdsource, int fddiff, int fddest)
{
    int rval = 0;
    _synctory_fheader_t header;
    uint8_t type;
    uint64_t index;
    unsigned char ibuf[9];
    _synctory_off_t offset;
    ssize_t rbytes;
    
    /* try to read header from diff file */
    if ((rval = _synctory_fh_getheader_fd(&header, fddiff)) != 0)
        return rval;
    
    /* position diff file pointer at beginning of data section */
    if ((offset = _synctory_file64_seek(fddiff, _SYNCTORY_FH_BYTES, SEEK_SET)) != _SYNCTORY_FH_BYTES)
        return errno;
    
    while ((rbytes = read(fddiff, ibuf, 9)) == 9)
    {
        type = *((uint8_t *)&ibuf[0]);
        index = _synctory_ntoh64(*((uint64_t *)&ibuf[1]));
        
        switch (type)
        {
            case _SYNCTORY_DIFF_BTYPE_CHUNK:
                _synctory_file64_bytecopy(fdsource, fddest, index * header.chunksize, header.chunksize);
                break;
                    
            case _SYNCTORY_DIFF_BTYPE_RAW:
                _synctory_file64_bytecopy(fddiff, fddest, _synctory_file64_seek(fddiff, 0, SEEK_CUR), index);
                break;
                    
            default:
                return -1;
        }
    }
    
    return rval;
}


int
_synctory_synth_create_fn(const char *sourcefile, const char *difffile, const char *destfile)
{
    int rval = 0;
    int fdsource, fddiff, fddest;
    
    /* open read-only file descriptor for source file */
    fdsource = _synctory_file64_open(sourcefile, O_RDONLY);
    if (fdsource < 0)
        return errno;
    
    /* open read-only file descriptor for diff file */
    fddiff = _synctory_file64_open(difffile, O_RDONLY);
    if (fddiff < 0)
        return errno;
    
    /* open write-only file descriptor for diff file */
    fddest = _synctory_file64_open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fddest < 0)
        return errno;
    
    rval = _synctory_synth_create_fd(fdsource, fddiff, fddest);
    
    _synctory_file64_close(fdsource);
    _synctory_file64_close(fddiff);
    _synctory_file64_close(fddest);
    
    return rval;
}
