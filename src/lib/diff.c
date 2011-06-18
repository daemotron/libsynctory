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
#include "_tree.h"

TREE_DEFINE(_tree_node_s, linkage)

void
__tree_node_delete(_tree_node_t *node, void *tree)
{
    TREE_REMOVE((_tree_t *)tree, _tree_node_s, linkage, node);
}



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
 * New implementation of synctory_diff which stores the fingerprint in an AVL
 * tree for faster lookups.
 * 
 * @Warning 
 * This implementation has increased memory requirements, particularly when 
 * dealing with bigger fingerprint files!
 */
int
_synctory_diff_create_fast(int fdfinger, int fdsource, int fddiff)
{
    int                         rval = 0, status = 0;
    int                         iflag = 1;
    int                         i;
    uint64_t                    index = 0;
    _synctory_fheader_t         finger_header, diff_header;
    _synctory_off_t             position;
    _synctory_off_t             lpos, curpos;
    _synctory_checksum_t        weaksum;
    _tree_t                     ftree = TREE_INITIALIZER(_tree_node_compare);
    uint32_t                    wsum;
    unsigned char              *strongsum1;
    unsigned char              *strongsum2;
    unsigned char              *buffer;
    unsigned char               hbuf[_SYNCTORY_FH_BYTES];
    unsigned char               wbuf[9];
    unsigned char               lchar = '\0';
    ssize_t                     rbytes;
    _synctory_fingerprint_iterctx_t ctx;
    
    /*
     * STEP 1
     * 
     * Check the given fingerprint file, identify its header etc.
     */
    
    /* get file header from fingerprint file */
    rval = _synctory_fh_getheader_fd(&finger_header, fdfinger);
    if (rval)
        return rval;
    
    /* check whether the fingerprint file is acutally a fingerprint */
    if (finger_header.type != _SYNCTORY_FH_FINGERPRINT)
        return -1;
    
    /*
     * STEP 2
     *
     * Gerenate the AVL tree
     */
    
    /* Initialize data structures */
    _synctory_fingerprint_iterctx_init(&ctx, finger_header.chunksize, finger_header.algo);
    strongsum1 = (unsigned char *)malloc(_synctory_strong_checksum_size(finger_header.algo));
    strongsum2 = (unsigned char *)malloc(_synctory_strong_checksum_size(finger_header.algo));
    if ((NULL == strongsum1) || (NULL == strongsum2))
    {
        free(strongsum1);
        free(strongsum2);
        return errno;
    }
    
    /* iterate over fingerprint file */
    while (0 == _synctory_fingerprint_read_iter_fd(fdfinger, &wsum, strongsum1, _synctory_strong_checksum_size(finger_header.algo), &ctx))
    {
        _tree_node_t *v = _tree_node_new(wsum);
        _tree_node_t *vv = TREE_SEARCH(&ftree, v);
        status = 0;
        if (vv)
        {
            _tree_node_append_payload(vv, index, strongsum1, _synctory_strong_checksum_size(finger_header.algo), &status);
        }
        else
        {
            TREE_APPEND(&ftree, _tree_node_new(wsum));
            _tree_node_t *vv = TREE_SEARCH(&ftree, v);
            _tree_node_append_payload(vv, index, strongsum1, _synctory_strong_checksum_size(finger_header.algo), &status);
        }
        if (status)
        {
            free(strongsum1);
            free(strongsum2);
            TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
            return status;
        }
        index++;
    }
    
     /* find out about the file size of the diff source file */
    position = _synctory_file64_seek(fdsource, 0, SEEK_END);
    if (position < 0)
    {
        free(strongsum1);
        free(strongsum2);
        TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
        return errno;
    }
    
    /* collect information for the resulting diff file */
    diff_header.algo = finger_header.algo;
    diff_header.chunksize = finger_header.chunksize;
    diff_header.filesize = (uint64_t)position;
    diff_header.type = _SYNCTORY_FH_DIFF;
    diff_header.version = _SYNCTORY_VERSION_NUM;
    
    /* generate the ready-to-write header inside a buffer */
    rval = _synctory_fh_setheader_bf(&diff_header, hbuf, _SYNCTORY_FH_BYTES);
    if (rval)
    {
        free(strongsum1);
        free(strongsum2);
        TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
        return rval;
    }
    
    /* make sure we're at the beginning of the result file */
    if (0 != _synctory_file64_seek(fddiff, 0, SEEK_SET))
    {
        free(strongsum1);
        free(strongsum2);
        TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
        return errno;
    }
    
    /* try to write the header information into the result file */
    rbytes = write(fddiff, hbuf, _SYNCTORY_FH_BYTES);
    if (rbytes != _SYNCTORY_FH_BYTES)
    {
        free(strongsum1);
        free(strongsum2);
        TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
        return ((errno != 0) ? errno : -1);
    }
    
    /* initialize buffers */
    buffer = (unsigned char *)malloc(diff_header.chunksize);
    if (NULL == buffer)
    {
        free(strongsum1);
        free(strongsum2);
        free(buffer);
        TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
        return ((errno != 0) ? errno : -1);
    }
    
    /*
     * initialize source file position pointers.
     * lpos points to the position after the last known chunk
     * curpos points to the current position.
     */
    lpos = curpos = 0;
    
    /* make sure we're at the beginning of the source file */
    if (0 != _synctory_file64_seek(fdsource, curpos, SEEK_SET))
    {
        free(strongsum1);
        free(strongsum2);
        free(buffer);
        TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
        return errno;
    }
    
    /* read chunks froms source file and process them */
    while ((rbytes = read(fdsource, buffer, diff_header.chunksize)) > 0)
    {
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
        
        _tree_node_t *w = _tree_node_new(_synctory_checksum_digest(&weaksum));
        _tree_node_t *ww = TREE_SEARCH(&ftree, w);
        
        if (ww)
        {
            int kflag = 0;
            for (i = 0; i < ww->payloads; i++)
            {
                _synctory_strong_checksum(buffer, rbytes, strongsum2, diff_header.algo);
                
                /* compare strong checksums */
                if (0 == _synctory_strong_checksum_compare(ww->payload[i].strong_checksum, strongsum2, _synctory_strong_checksum_size(diff_header.algo)))
                {
                    /* bingo! set kflag to true and exit loop */
                    kflag = 1;
                    break;
                }
            }
            if (kflag)
            {
                /* first we need to check whether there are any unmatched bytes to save as "raw" */
                if (lpos != curpos)
                    __synctory_diff_flush_raw_fd(fdsource, fddiff, lpos, curpos);
                
                /* now take care of the identified chunk */
                wbuf[0] = (unsigned char)_SYNCTORY_DIFF_BTYPE_CHUNK;
                *((uint64_t *)&wbuf[1]) = _synctory_hton64(ww->payload[i].position);
                if (write(fddiff, wbuf, 9) != 9)
                {
                    free(strongsum1);
                    free(strongsum2);
                    free(buffer);
                    TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
                    return ((errno != 0) ? errno : -1);
                }
                
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
            
        }
        else
        {
            /* go one byte ahead and try again */
            curpos++;
            lchar = buffer[0];
        }

        if (curpos != _synctory_file64_seek(fdsource, curpos, SEEK_SET))
        {
            free(strongsum1);
            free(strongsum2);
            free(buffer);
            TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
            return errno;
        }
    }
    
     /* any raw bytes left to flush down the toilet? */
    if (lpos != curpos)
        __synctory_diff_flush_raw_fd(fdsource, fddiff, lpos, curpos);
    
    /* destroy structures and the tree */
    free(strongsum1);
    free(strongsum2);
    free(buffer);
    TREE_FWD_APPLY(&ftree, __tree_node_delete, &ftree);
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
