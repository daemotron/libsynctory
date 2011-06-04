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


#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include "version.h"

#include "_checksum.h"
#include "_diff.h"
#include "_endianess.h"
#include "_fingerprint.h"
#include "_fheader.h"
#include "_file64.h"


/**
 * Transfer bytes between lpos and curpos from fdsource to fddiff.
 */
int
__synctory_diff_flush_raw_fd(int fdsource, int fddest, _synctory_off_t lpos, _synctory_off_t curpos)
{
    int rval = 0;
    unsigned char wbuf[9];
    
    /* prepare a raw chunk header inicating the size of the raw chunk */
    wbuf[0] = (unsigned char)_SYNCTORY_DIFF_BTYPE_RAW;
    *((uint64_t *)&wbuf[1]) = _synctory_hton64(curpos - lpos);
    
    /* write the raw chunk header to the diff result file */
    if (write(fddest, wbuf, 9) != 9)
        return ((errno != 0) ? errno : -1);
    
    /* copy the raw byte chunk */
    _synctory_file64_bytecopy(fdsource, fddest, lpos, curpos - lpos);
    
    return rval;
}


/**
 * Create a synctory diff file from a given fingerprint and a source file descriptor.
 * The fingerprint will be compared with the source file; recognized differences
 * will be written to the diff file descriptor in a libsynctory-readable format.
 */
int
_synctory_diff_create_fd(int fdfinger, int fdsource, int fddiff)
{
    int kflag, rval = 0;
    int iflag = 1;
    uint64_t index;
    unsigned char hbuf[_SYNCTORY_FH_BYTES];
    unsigned char *buffer;
    unsigned char wbuf[9];
    unsigned char lchar = '\0';
    _synctory_fheader_t finger_header, diff_header;
    _synctory_off_t position;
    _synctory_off_t lpos, curpos;
    _synctory_checksum_t weaksum;
    uint32_t wsum;
    unsigned char *strongsum[2];
    _synctory_fingerprint_iterctx_t ctx;
    ssize_t rbytes;
    
    /* initialize weak checksum structure */
    _synctory_checksum_init(&weaksum);
    
    /* try to read header from fingerprint file */
    rval = _synctory_fingerprint_fetchheader_fd(fdfinger, &finger_header);
    if (rval)
        return rval;
    
    /* check whether the fingerprint file is acutally a fingerprint */
    if (finger_header.type != _SYNCTORY_FH_FINGERPRINT)
        return -1;
    
    /* find out about the file size of the diff source file */
    position = _synctory_file64_seek(fdsource, 0, SEEK_END);
    if (position < 0)
        return errno;
    
    /* collect information for the resulting diff file */
    diff_header.algo = finger_header.algo;
    diff_header.chunksize = finger_header.chunksize;
    diff_header.filesize = (uint64_t)position;
    diff_header.type = _SYNCTORY_FH_DIFF;
    diff_header.version = _SYNCTORY_VERSION_NUM;
    
    /* generate the ready-to-write header inside a buffer */
    rval = _synctory_fh_setheader_bf(&diff_header, hbuf, _SYNCTORY_FH_BYTES);
    if (rval)
        return rval;
    
    /* make sure we're at the beginning of the result file */
    if (0 != _synctory_file64_seek(fddiff, 0, SEEK_SET))
        return errno;
    
    /* try to write the header information into the result file */
    rbytes = write(fddiff, hbuf, _SYNCTORY_FH_BYTES);
    if (rbytes != _SYNCTORY_FH_BYTES)
        return ((errno != 0) ? errno : -1);
    
    /* initialize buffers */
    buffer = (unsigned char *)malloc(diff_header.chunksize);
    strongsum[0] = (unsigned char *)malloc(_synctory_strong_checksum_size(diff_header.algo));
    strongsum[1] = (unsigned char *)malloc(_synctory_strong_checksum_size(diff_header.algo));
    if ((NULL == buffer) || (NULL == strongsum[0]) || (NULL == strongsum[1]))
        return errno;

    /*
     * initialize source file position pointers.
     * lpos points to the position after the last known chunk
     * curpos points to the current position.
     */
    lpos = curpos = 0;
    
    /* make sure we're at the beginning of the source file */
    if (0 != _synctory_file64_seek(fdsource, curpos, SEEK_SET))
        return errno;
    
    /* read chunks froms source file and process them */
    while ((rbytes = read(fdsource, buffer, diff_header.chunksize)) > 0)
    {
        /* initialize helper variables */
        kflag = 0;
        index = 0;
        _synctory_fingerprint_iterctx_init(&ctx, diff_header.chunksize, diff_header.algo);

        if (iflag || (rbytes < diff_header.chunksize))
        {
            _synctory_checksum_init(&weaksum);
            _synctory_checksum_update(&weaksum, buffer, rbytes);
            iflag = 0;
        }
        else
        {
            _synctory_checksum_rotate(&weaksum, lchar, buffer[diff_header.chunksize - 1]);
        }
        
        /* iterate over all chunk checksums in the fingerprint file */
        while (0 == _synctory_fingerprint_read_iter_fd(fdfinger, &wsum, strongsum[0], _synctory_strong_checksum_size(diff_header.algo), &ctx))
        {
            /* check whether the weak sum matches */
            if (wsum == _synctory_checksum_digest(&weaksum))
            {
                /* compute the strong checksum (only done when weak one already matched */
                _synctory_strong_checksum(buffer, rbytes, strongsum[1], diff_header.algo);
                
                /* compare strong checksums */
                if (0 == _synctory_strong_checksum_compare(strongsum[0], strongsum[1], _synctory_strong_checksum_size(diff_header.algo)))
                {
                    /* bingo! set kflag to true and exit loop */
                    kflag = 1;
                    break;
                }
            }
                /* no match => increase index counter */
                index++;
        }

        if (kflag)
        {
            /* first we need to check whether there are any unmatched bytes to save as "raw" */
            if (lpos != curpos)
                __synctory_diff_flush_raw_fd(fdsource, fddiff, lpos, curpos);
            
            /* now take care of the identified chunk */
            wbuf[0] = (unsigned char)_SYNCTORY_DIFF_BTYPE_CHUNK;
            *((uint64_t *)&wbuf[1]) = _synctory_hton64(index);
            if (write(fddiff, wbuf, 9) != 9)
                return ((errno != 0) ? errno : -1);
            
            /* continue after the identified chunk */
            lpos = curpos = (curpos + diff_header.chunksize);
            iflag = 1;
        }
        else
        {
            /* go one byte ahead and try again */
            curpos++;
            lchar = buffer[0];
        }
        if (curpos != _synctory_file64_seek(fdsource, curpos, SEEK_SET))
            return errno;

    }
    
    /* any raw bytes left to flush down the toilet? */
    if (lpos != curpos)
        __synctory_diff_flush_raw_fd(fdsource, fddiff, lpos, curpos);
    
    return rval;
}


/**
 * Create a synctory diff file from a given fingerprint and a source file name.
 * The fingerprint will be compared with the source file; recognized differences
 * will be written to the diff file name in a libsynctory-readable format.
 */
int
_synctory_diff_create_fn(const char *fingerprint, const char *sourcefile, const char *difffile)
{
    int rval = 0;
    int fdfinger, fdsource, fddiff;
    
    /* open read-only file descriptor for fingerprint file */
    fdfinger = _synctory_file64_open(fingerprint, O_RDONLY);
    if (fdfinger < 0)
        return errno;
    
    /* open read-only file descriptor for source file */
    fdsource = _synctory_file64_open(sourcefile, O_RDONLY);
    if (fdsource < 0)
        return errno;
    
    /* open write-only file descriptor for diff file */
    fddiff = _synctory_file64_open(difffile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fddiff < 0)
        return errno;
    
    rval = _synctory_diff_create_fd(fdfinger, fdsource, fddiff);
    
    _synctory_file64_close(fdfinger);
    _synctory_file64_close(fdsource);
    _synctory_file64_close(fddiff);
    
    return rval;
}
