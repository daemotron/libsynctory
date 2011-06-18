/*-
 * Copyright (c) 2010, 2011 Daemotron <mail@daemotron.net>
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


#ifndef __LIBSYNCTORY_H
#define __LIBSYNCTORY_H


#include <stddef.h>
#include <stdint.h>


/**
 * Available strong checksum algorithms
 * 
 * The strong checksum algorithm is used during fingerprint creation and for
 * verification purposes. Since the strong checksum also becomes part of the
 * fingerprint, it is recommended to find a balance between collision safety,
 * runtime performance and fingerprint size.
 */
typedef enum
{
    synctory_algo_rmd160      = 0x10,
    synctory_algo_sha1        = 0x20
} synctory_algo_t;


/**
 * The libsynctory context object
 * 
 * The libsynctory context object (LCO) is used to configure the behaviour of
 * libsynctory within the given boundaries.
 * 
 * chunk_size           The chunk size to use when creating a fingerprint. When 
 *                      operating on existing fingerprints, this option has
 *                      no effect, since the chunk size is automatically detected
 *                      in this case.
 * 
 * checksum_algorithm   The strong checksum algorthim to use when creating a
 *                      fingerprint. When operating on existing fingerprints, this
 *                      option has no effect, since the algorithm used to generate
 *                      a fingerprint is detected automatically from the fingerprint
 *                      header information.
 */
typedef struct
{
    uint16_t chunk_size;
    synctory_algo_t checksum_algorithm;
} synctory_ctx_t;


/**
 * Get library version information.
 * 
 * This function writes the version information as numeric type into the
 * provided uint64_t memory, allowing comparisons using standard relational
 * operators. Newer versions always have greater version numbers than older 
 * versions of libsynctory. 
 * 
 * The version information string is written into the provided buffer. 
 * The provided buffer should have a minimum size of synctory_version_bytes()
 * - otherwise it would not be able to contain the entire version string.
 * 
 * To skip retrieval of one of the offered formats, simply provide a
 * NULL pointer instead of a valid pointer for the format you do not want
 * to retrieve.
 */
extern void synctory_version(uint64_t *num, void *buffer, size_t len);


/**
 * Get the size requirement in bytes for a buffer to contain the
 * libsynctory version string. Included in the requirement is the
 * space for the string terminator byte.
 */
extern size_t synctory_version_bytes(void);


/**
 * Initialize synctory context object.
 * 
 * The context properties are filled with default values set at compile
 * time for libsynctory. After initialization, some properties of the
 * context object can be changed to meet specific needs.
 */
extern void synctory_init(synctory_ctx_t *ctx);


/**
 * Create fingerprint from source file and store it in destination file.
 * 
 * As arguments, this function takes the source and destination file. Both
 * can be provided either as file descriptor or as path string. The file
 * descriptor has a higher priority, i. e. if both file descriptor and path
 * name are provided, only the file descriptor will be used; the pathname
 * is ignored in this case.
 * 
 * It is possible to indicate mixed arguments, e. g. indicate the source file
 * as path name and the destination file as file descriptor.
 * 
 * To skip one form of indication, simply provide a NULL pointer for path names,
 * or a negative integer (usually -1) for the file descriptor.
 */
extern int synctory_fingerprint(synctory_ctx_t *ctx, int source_fd, int dest_fd, const char *source_file, const char *dest_file);


/**
 * Create a diff file based on a fingerprint and a source file.
 * 
 * The diff represents the difference between a file f1 and another file f2.
 * f1 is actually neither known nor available, only its fingerprint is required
 * to calculate the difference between f1 and f2. f2 however must be known and
 * present.
 * 
 * This function takes three arguments:
 * 
 *   source file => path or descriptor of file f2
 *   fingerprint => path or descriptor of fingerprint from f1
 *   destination => path or descriptor to store the diff result to
 *
 * All arguments can be provided either as file descriptor or as path string. The 
 * file descriptor has a higher priority, i. e. if both file descriptor and path
 * name are provided, only the file descriptor will be used; the pathname
 * is ignored in this case.
 * 
 * It is possible to indicate mixed arguments, e. g. indicate the source file
 * as path name and the destination file as file descriptor.
 * 
 * To skip one form of indication, simply provide a NULL pointer for path names,
 * or a negative integer (usually -1) for the file descriptor.
 */
extern int synctory_diff(int source_fd, int dest_fd, int fingerprint_fd, const char *source_file, const char *dest_file, const char *fingerprint_file);


/**
 * Create a diff file using a low memory profile.
 * 
 * This function operates in the same way as synctory_diff; the only difference is
 * that its memory footprint is not dependend on the amount of fingerprint chunks
 * to process.
 * 
 * Therefore this function is useful to process extremely large fingerprint files.
 * It can also be used in environments with limited memory availability.
 */
extern int synctory_diff_lomem(int source_fd, int dest_fd, int fingerprint_fd, const char *source_file, const char *dest_file, const char *fingerprint_file);


/**
 * Synthesize a file based on a diff and a source file.
 * 
 * Using this function, it is possible to synthesize the file f2 (see comments
 * above concerning synctory_diff) from the file f1 and the diff
 * created from the fingerprint of f1 and the contents of f2.
 * 
 * This function takes three arguments:
 * 
 *   source file => path or descriptor of file f1
 *   diff        => path or descriptor of diff between f1 and f2
 *   destination => path or descriptor to store the synthesized result to (equals f2)
 *
 * All arguments can be provided either as file descriptor or as path string. The 
 * file descriptor has a higher priority, i. e. if both file descriptor and path
 * name are provided, only the file descriptor will be used; the pathname
 * is ignored in this case.
 * 
 * It is possible to indicate mixed arguments, e. g. indicate the source file
 * as path name and the destination file as file descriptor.
 * 
 * To skip one form of indication, simply provide a NULL pointer for path names,
 * or a negative integer (usually -1) for the file descriptor.
 */
extern int synctory_synth(int source_fd, int dest_fd, int diff_fd, const char *source_file, const char *dest_file, const char *diff_file);

#endif /* __LIBSYNCTORY_H */
