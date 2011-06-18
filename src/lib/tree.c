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


#include <stdlib.h>

#include "_tree.h"


int
_tree_node_compare(_tree_node_t *lhs, _tree_node_t *rhs)
{
    return (lhs->checksum >= rhs->checksum) - (rhs->checksum >= lhs->checksum);
}


_tree_node_t *
_tree_node_new(uint32_t checksum)
{
    _tree_node_t *self = (_tree_node_t *)calloc(1, sizeof(_tree_node_t));
    if (NULL == self)
        return NULL;
    
    self->checksum = checksum;
    self->payload = NULL;
    self->payloads = 0;
    
    return self;
}


_tree_t *
_tree_new(int (*compare)(_tree_node_t *lhs, _tree_node_t *rhs))
{
    _tree_t *self = (_tree_t *)calloc(1, sizeof(_tree_t));
    if (NULL == self)
        return NULL;
    
    TREE_INIT(self, compare);
    return self;
}


/*void
_tree_node_delete(_tree_node_t *node, void *tree)
{
    TREE_REMOVE((_tree_t *)tree, _tree_node_s, linkage, node);
}*/
