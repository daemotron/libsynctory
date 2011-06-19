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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "tests.h"
#include "helpers.h"


void test_performance(const test_ctx_t *ctx, int *status)
{
    int                 i, j, rval, k;
    clock_t start,      stop;
    synctory_ctx_t      sctx;
    char               *filenames[20] = { NULL };
    char               *labels[4] = { "small", "big", "huge", "giant" };
    char               *tsizes[4] = { "512 KiB", "512 MiB", "2.5 GiB", "5.0 GiB" };
    size_t              fnamesize;
    off_t               filesizes[4];
    hlp_progress_t      pgctx;
    off_t               modpos[5000];
    unsigned char       obytes[5000];
    unsigned char       mbytes[5000];
    unsigned int        amount[4];
    clock_t             fprt[4][5];
    clock_t             diff[4][5];
    clock_t             synt[4][5];
    double              fp[4];
    double              df[4];
    double              sy[4];
    const int           tests = 4;
    
    synctory_init(&sctx);
    
    filesizes[0] = 0x7d000;             /* small:       512 KiB         */
    filesizes[1] = 0x1f400000;          /* big:         512 MiB         */
    filesizes[2] = 0xa0000000;          /* huge:        2.5 GiB         */
    filesizes[3] = 0x140000000;         /* giant:       5.0 GiB         */
    
    amount[0] = 10;
    amount[1] = 1000;
    amount[2] = 2000;
    amount[3] = 5000;
    
    /*
     * PREPARE FILE NAME TABLE
     */
    fnamesize = strlen(ctx->workdir) + strlen("test_perf.orig.01") + 2;
    
    for (i = 0; i < 20; i++)
    {
        filenames[i] = (char *)malloc(fnamesize);
        if (NULL == filenames[i])
        {
            *status = errno;
            for(; i >= 0; i--)
                free(filenames[i]);
            return;
        }
    }
    
    hlp_path_join(ctx->workdir, "test_perf.orig.01", filenames[0], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.orig.02", filenames[1], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.orig.03", filenames[2], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.orig.04", filenames[3], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.modf.01", filenames[4], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.modf.02", filenames[5], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.modf.03", filenames[6], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.modf.04", filenames[7], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.fprt.01", filenames[8], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.fprt.02", filenames[9], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.fprt.03", filenames[10], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.fprt.04", filenames[11], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.diff.01", filenames[12], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.diff.02", filenames[13], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.diff.03", filenames[14], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.diff.04", filenames[15], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.synt.01", filenames[16], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.synt.02", filenames[17], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.synt.03", filenames[18], fnamesize);
    hlp_path_join(ctx->workdir, "test_perf.synt.04", filenames[19], fnamesize);
    
    /*
     * PREPARE SAMPLE FILES
     */
    hlp_progress_init(&pgctx);
    pgctx.character = '.';
    pgctx.stream = stdout;
    pgctx.width = 30;
    
    for (i = 0; i < tests; i++)
    {
        printf("  preparing %s test file ", labels[i]);
        for (j = 0; j < (5 - (int)strlen(labels[i])); j++)
            printf(" ");
        printf("(%s)   ", tsizes[i]);
        fflush(stdout);
        rval = hlp_file_bytecopy(ctx->random_device, filenames[i], filesizes[i], &pgctx);
        if (rval)
            printf(" failed\n");
        else
            printf(" success\n");
    }

    for (i = 0; i < tests; i++)
    {
        printf("  preparing %s modified file ", labels[i]);
        for (j = 0; j < (5 - (int)strlen(labels[i])); j++)
            printf(" ");
        printf("        ");
        fflush(stdout);
        rval = hlp_file_bytecopy(filenames[i], filenames[i+4], filesizes[i], &pgctx);
        if (rval)
            printf(" failed\n");
        else
        {
            rval = hlp_file_randmod(filenames[i+4], amount[i], modpos, obytes, mbytes);
            if (rval)
                printf(" failed\n");
            else
                printf(" success\n");
        }
    }
    
    printf("\n  fingerprint benchmark\n");
    for (i = 0; i < tests; i++)
    {
        printf("    %s file benchmark                ", labels[i]);
        for (j = 0; j < (5 - (int)strlen(labels[i])); j++)
            printf(" ");
        fflush(stdout);
        for (j = 0; j < 5; j++)
        {
            start = clock();
            rval = synctory_fingerprint(&sctx, -1, -1, filenames[i], filenames[i+8]);
            stop = clock();
            fprt[i][j] = stop - start;
            if (rval)
            {
                for (k = 0; k < (5-j); k++)
                    printf("......");
                printf(" failed\n");
                break;
            }
            else
            {
                printf("......");
                fflush(stdout);
            }
        }
        if (!rval)
            printf(" success\n");
    }
    
    printf("\n  diff benchmark\n");
    for (i = 0; i < tests; i++)
    {
        printf("    %s file benchmark                ", labels[i]);
        for (j = 0; j < (5 - (int)strlen(labels[i])); j++)
            printf(" ");
        fflush(stdout);
        for (j = 0; j < 5; j++)
        {
            start = clock();
            rval = synctory_diff(-1, -1, -1, filenames[i+4], filenames[i+12], filenames[i+8]);
            stop = clock();
            diff[i][j] = stop - start;
            if (rval)
            {
                for (k = 0; k < (5-j); k++)
                    printf("......");
                printf(" failed\n");
                break;
            }
            else
            {
                printf("......");
                fflush(stdout);
            }
        }
        if (!rval)
            printf(" success\n");
    }

    printf("\n  synth benchmark\n");
    for (i = 0; i < tests; i++)
    {
        printf("    %s file benchmark                ", labels[i]);
        for (j = 0; j < (5 - (int)strlen(labels[i])); j++)
            printf(" ");
        fflush(stdout);
        for (j = 0; j < 5; j++)
        {
            start = clock();
            rval = synctory_synth(-1, -1, -1, filenames[i], filenames[i+16], filenames[i+12]);
            stop = clock();
            synt[i][j] = stop - start;
            if (rval)
            {
                for (k = 0; k < (5-j); k++)
                    printf("......");
                printf(" failed\n");
                break;
            }
            else
            {
                printf("......");
                fflush(stdout);
            }
        }
        if (!rval)
            printf(" success\n");
    }
    
    /* generate statistics results */
    for (i = 0; i < tests; i++)
    {
        fp[i] = ((((double)(fprt[i][0] + fprt[i][1] + fprt[i][2] + fprt[i][3] + fprt[i][4])) / 5.0) / (double)CLOCKS_PER_SEC);
        df[i] = ((((double)(diff[i][0] + diff[i][1] + diff[i][2] + diff[i][3] + diff[i][4])) / 5.0) / (double)CLOCKS_PER_SEC);
        sy[i] = ((((double)(synt[i][0] + synt[i][1] + synt[i][2] + synt[i][3] + synt[i][4])) / 5.0) / (double)CLOCKS_PER_SEC);
    }
    
    printf("\n\n  benchmark results\n\n");
    printf("  +-------------+---------+---------+---------+---------+\n");
    printf("  |             | small   | big     | huge    | giant   |\n");
    printf("  +-------------+---------+---------+---------+---------+\n");
    printf("  | fingerprint | %#7.2lf | %#7.2lf | %#7.2lf | %#7.2lf |\n", fp[0], fp[1], fp[2], fp[3]);
    printf("  +-------------+---------+---------+---------+---------+\n");
    printf("  | diff        | %#7.2lf | %#7.2lf | %#7.2lf | %#7.2lf |\n", df[0], df[1], df[2], df[3]);
    printf("  +-------------+---------+---------+---------+---------+\n");
    printf("  | synth       | %#7.2lf | %#7.2lf | %#7.2lf | %#7.2lf |\n", sy[0], sy[1], sy[2], sy[3]);
    printf("  +-------------+---------+---------+---------+---------+\n");

    if (ctx->cleanup)
    {
        for (i = 0; i < tests; i++)
        {
            unlink(filenames[i]);
            unlink(filenames[i+4]);
            unlink(filenames[i+8]);
            unlink(filenames[i+12]);
            unlink(filenames[i+16]);
        }
    }
    
    for (i = 0; i < 20; i++)
    {
        free(filenames[i]);
    }
    
    *status = rval;
}
