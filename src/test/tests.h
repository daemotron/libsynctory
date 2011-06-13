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


#ifndef __SYNCTORY_TESTS_H_
#define __SYNCTORY_TESTS_H_

#include "test.h"


/*
 * NOTE
 * 
 * Please list all test function prototypes here.
 * Do not forget to include them in the test array in
 * tests.c
 */

void test_version(const test_ctx_t *ctx, int *status);
void test_fingerprint(const test_ctx_t *ctx, int *status);
void test_diff(const test_ctx_t *ctx, int *status);



#endif /* __SYNCTORY_TESTS_H_ */
