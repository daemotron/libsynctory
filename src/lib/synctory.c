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

#include <synctory.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "version.h"

extern uint64_t
synctory_version_num(void)
{
    return LIBSYNCTORY_VERSION_NUM;
}


extern size_t
synctory_version_bytes(void)
{
    return strlen(PACKAGE_VERSION) + 1;
}


extern void
synctory_version_str(void *buffer, size_t len)
{
    size_t maxlen = (strlen(PACKAGE_VERSION) > (len - 1) ? len - 1 : strlen(PACKAGE_VERSION));
    strncpy((char *)buffer, PACKAGE_VERSION, maxlen);
    buffer[maxlen] = '\0';
}