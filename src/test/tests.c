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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "tests.h"



/*
 * NOTE
 * 
 * Please add the tests that should actually be run to this
 * list. Please do not forget to add the prototype to tests.h
 * and make sure the source code file is included in the
 * CMakeLists.txt file for this directory.
 * 
 * Tests will be run exactly in the order they're indicated in
 * the array below!
 */
static test_t tests[] =
{
    { "libsynctory version information", test_version },
    { "libsynctory fingerprint test", test_fingerprint },
    { "libsynctory diff test", test_diff },
    { "libsynctory synth test", test_synth },
    { "libsynctory performance test", test_performance },
    
    /* terminator of the tests array. Keep this under all circumstances! */
    { NULL, NULL },
};



/*
 * NOTE
 * 
 * Below you'll find only functions needed to dispatch and run
 * the tests. Don't change them, or you will break the test
 * framework!
 */


int test_max(void)
{
    int test_total = 0;
    while (tests[test_total].name != NULL)
        test_total++;
    return test_total;
}


void test_get_name(int id, char *buffer, size_t len)
{
    if (id < test_max())
        strncpy(buffer, tests[id].name, ((len < strlen(tests[id].name) + 1) ? len : strlen(tests[id].name) + 1));
    else
        buffer = NULL;
}


void test_run(int id, const test_ctx_t *ctx, int *status)
{
    if (id < test_max())
        (*tests[id].test_function)(ctx, status);
    else
        *status = ERANGE;
}
