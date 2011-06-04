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



#ifndef __SYNCTORY_TEST_H_
#define __SYNCTORY_TEST_H_


#include <stddef.h>
#include <string.h>


typedef struct
{
    char workdir[2048];
    char random_device[2048];
} test_ctx_t;


#define test_init(ctx) {                                \
    memset((ctx)->workdir, (int)'\0', 2048);            \
    memset((ctx)->random_device, (int)'\0', 2048);      \
}


typedef struct
{
    char *name;
    void (*test_function)(const test_ctx_t *ctx, int *status);
} test_t;


int  test_max(void);
void test_get_name(int id, char *buffer, size_t len);
void test_run(int id, const test_ctx_t *ctx, int *status);

#endif /* __SYNCTORY_TEST_H_ */
