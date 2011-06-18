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


#ifndef __LIBSYNCTORY_TREE_H_
#define __LIBSYNCTORY_TREE_H_


#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"


typedef struct
{
    uint64_t position;
    unsigned char *strong_checksum;
} _tree_payload_t;


typedef struct _tree_node_s
{
    uint32_t                    checksum;       /* identifying element, weak checksum   */
    int                         payloads;       /* number of payloads in payload array  */
    _tree_payload_t            *payload;        /* payload array                        */
    TREE_ENTRY(_tree_node_s)    linkage;        /* magic from AVL tree implementation   */
} _tree_node_t;


#define _tree_node_append_payload(node, pos, checksum, size, status) {                                                  \
    (node)->payloads++;                                                                                                 \
    (node)->payload = (_tree_payload_t *)realloc((node)->payload, (node)->payloads * sizeof(_tree_payload_t));          \
    if (NULL == (node)->payload) {                                                                                      \
        free((node)->payload);                                                                                          \
        *(status) = errno;                                                                                              \
    }                                                                                                                   \
    else {                                                                                                              \
        (node)->payload[(node)->payloads - 1].position = (pos);                                                         \
        (node)->payload[(node)->payloads - 1].strong_checksum = (unsigned char *)malloc((size));                        \
        if (NULL == (node)->payload[(node)->payloads - 1].strong_checksum) {                                            \
            free((node)->payload[(node)->payloads - 1].strong_checksum);                                                \
            *(status) = errno;                                                                                          \
        }                                                                                                               \
        else {                                                                                                          \
            memcpy((node)->payload[(node)->payloads - 1].strong_checksum, (checksum), (size));                          \
            *(status) = 0;                                                                                              \
        }                                                                                                               \
    }                                                                                                                   \
}


typedef TREE_HEAD(_tree_head_s, _tree_node_s) _tree_t;

#define TREE_SEARCH(head, elm)                  TREE_FIND((head), _tree_node_s, linkage, (elm))
#define TREE_APPEND(head, elm)                  TREE_INSERT((head), _tree_node_s, linkage, (elm))
#define TREE_FWD_APPLY(tree, function, data)    TREE_FORWARD_APPLY((tree), _tree_node_s, linkage, (function), (data))


int             _tree_node_compare(_tree_node_t *lhs, _tree_node_t *rhs);
_tree_node_t   *_tree_node_new(uint32_t checksum);
_tree_t        *_tree_new(int (*compare)(_tree_node_t *lhs, _tree_node_t *rhs));
/*void            _tree_node_delete(_tree_node_t *node, void *tree);*/

#endif /* __LIBSYNCTORY_TREE_H_ */
