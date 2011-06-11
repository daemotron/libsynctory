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
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <synctory.h>

#include "helpers.h"
#include "test.h"
#include "tests.h"


#define DEFAULT_DIR     "."
#define DEFAULT_RANDOM  "/dev/urandom"


void usage(void)
{
    printf(
        "Usage: synctory_test [options]\n\n"
        "Options:\n"
        "  -C           Don't clean up temporary files\n"
        "  -d <path>    Use <path> for temporary files. Defaults to %s\n"
        "  -r <path>    Use <path> to read random bytes. Defaults to %s\n\n",
        DEFAULT_DIR, DEFAULT_RANDOM
    );
}



int main(int argc, char **argv)
{
    int ch, test_cur, test_total, i;
    size_t maxlen = 0;
    int *result;
    test_ctx_t ctx;
    char namebuf[100] = { '\0' };
    
    test_init(&ctx);
    
    while ((ch = getopt(argc, argv, "hCd:r:")) != -1)
    {
        switch (ch)
        {
            case 'h':
                usage();
                return EXIT_SUCCESS;
            case 'C':
                ctx.cleanup = 0;
                break;
            case 'd':
                if (NULL != optarg)
                    hlp_abs_path(optarg, ctx.workdir, 2048);
                else
                    return EXIT_FAILURE;
                break;
            case 'r':
                if (NULL != optarg)
                    hlp_abs_path(optarg, ctx.random_device, 2048);
                else
                    return EXIT_FAILURE;
                break;
            default:
                printf("\n");
                usage();
                return EXIT_FAILURE;
        }
    }
    
    if (strlen(ctx.workdir) == 0)
        hlp_abs_path(DEFAULT_DIR, ctx.workdir, 2048);
    
    if (strlen(ctx.random_device) == 0)
        hlp_abs_path(DEFAULT_RANDOM, ctx.random_device, 2048);
    
    
    /* determine total number of tests */
    test_total = test_max();
    
    result = (int *)malloc(test_total * sizeof(int));
    if (NULL == result)
    {
        hlp_report_error(errno);
        return EXIT_FAILURE;
    }
    
    
    /* Standard message */
    printf("\n=============================\nlibsynctory testing framework\n=============================\n\n");
    
    /* display settings */
    printf("Settings for current test run:\n------------------------------\n\n");
    printf("Working directory:          %s\n", ctx.workdir);
    printf("Random device:              %s\n", ctx.random_device);
    printf("Delete temporary files:     ");
    if (ctx.cleanup)
        printf("YES\n");
    else
        printf("NO\n");
    printf("\n\n\n");
    
    /* run tests */
    for (test_cur = 0; test_cur < test_total; test_cur++)
    {
        memset(namebuf, (int)'\0', 100);
        test_get_name(test_cur, namebuf, 100);
        printf("Test %02d of %02d: %s\n---------------", test_cur + 1, test_total, namebuf);
        for (i = 0; i < (int)strlen(namebuf); i++)
            printf("-");
        printf("\n\n");
        
        if (strlen(namebuf) > maxlen)
            maxlen = strlen(namebuf);
        
        test_run(test_cur, &ctx, &result[test_cur]);
        
        printf("\nTest %02d of %02d ", test_cur + 1, test_total);
        if (result[test_cur])
        {
            printf("failed: ");
            hlp_report_error(result[test_cur]);
            printf("\n\n");
        }
        else
            printf("passed.\n\n\n");
    }
    
    printf("\nTest Summary\n============\n\n");
    printf("Test No.  Description                                                     Result\n"
           "--------  --------------------------------------------------------------  ------\n");
    
    for (test_cur = 0; test_cur < test_total; test_cur++)
    {
        memset(namebuf, (int)'\0', 100);
        test_get_name(test_cur, namebuf, 100);
        printf("%02d        ", test_cur + 1);
        if (strlen(namebuf) > 62)
            printf("%62s  ", namebuf);
        else
        {
            printf("%s", namebuf);
            for (i = 0; i < (62 - (int)strlen(namebuf)); i++)
                printf(" ");
            printf("  ");
        }
        if (result[test_cur])
            printf("failed\n");
        else
            printf("passed\n");
    }
    printf("\n");
    return EXIT_SUCCESS;
}
