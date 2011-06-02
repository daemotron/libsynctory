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


#ifndef __LIBSYNCTORY_DEFAULT_H
#define __LIBSYNCTORY_DEFAULT_H

/**
 * This file defines some basic defaults needed for initialization.
 * All defaults can be overridden by corresponding context manipulating
 * operations offered by synctory.c
 */


/* 
 * Default Chunk Size
 * 
 * Relevant for fingerprint creation
 */
#define SYNCTORY_DEFAULT_CHUNKSIZE      512U


/*
 * Default Strong Checksum Algorithm
 * 
 * Relevant for fingerprint creation
 * 
 * 0x10 => RIPEMD_160
 * 0x20 => SHA1
 * 
 * Valid algorithm constants are defined in synctory.h
 */
#define SYNCTORY_DEFAULT_CHECKSUM       0x10

#endif /* __LIBSYNCTORY_DEFAULT_H */
