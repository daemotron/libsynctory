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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "tests.h"
#include "helpers.h"


#define __TEST_DF_SFILE_SIZE    0x7d000ULL      /* 512 KiB      */


void test_diff(const test_ctx_t *ctx, int *status)
{
    char *filename_o = NULL, *filename_m = NULL, *filename_fp = NULL, *filename_df = NULL, *filename_dl = NULL;
    hlp_progress_t pgctx;
    size_t fnamesize;
    size_t fnamesize_fp;
    size_t fnamesize_df;
    size_t fnamesize_dl;
    int rval;
    synctory_ctx_t sctx;
    off_t modpos[5];
    unsigned char obytes[5];
    unsigned char mbytes[5];
    clock_t start, stop;
    
    synctory_init(&sctx);
    
    fnamesize = strlen(ctx->workdir) + 16;
    fnamesize_fp = strlen(ctx->workdir) + 19;
    fnamesize_df = strlen(ctx->workdir) + 26;
    fnamesize_dl = strlen(ctx->workdir) + 27;
    
    filename_o = (char *)malloc(fnamesize);
    filename_m = (char *)malloc(fnamesize);
    filename_fp = (char *)malloc(fnamesize_fp);
    filename_df = (char *)malloc(fnamesize_df);
    filename_dl = (char *)malloc(fnamesize_dl);
    
    if ((NULL == filename_o) || (NULL == filename_m) || (NULL == filename_fp) || (NULL == filename_df) || (NULL == filename_dl))
    {
        *status = errno;
        free(filename_o);
        free(filename_m);
        free(filename_fp);
        free(filename_df);
        free(filename_dl);
        return;
    }
    
    hlp_path_join(ctx->workdir, "test_diff.orig", filename_o, fnamesize);
    hlp_path_join(ctx->workdir, "test_diff.modf", filename_m, fnamesize);
    hlp_path_join(ctx->workdir, "test_diff.orig.fp", filename_fp, fnamesize_fp);
    hlp_path_join(ctx->workdir, "test_diff.modf.diff_fast", filename_df, fnamesize_df);
    hlp_path_join(ctx->workdir, "test_diff.modf.diff_lomem", filename_dl, fnamesize_dl);
    
     /* prepare test file */
    hlp_progress_init(&pgctx);
    pgctx.character = '.';
    pgctx.stream = stdout;
    pgctx.width = 30;
    
    printf("  preparing small test file (512 KiB)   ");
    rval = hlp_file_bytecopy(ctx->random_device, filename_o, __TEST_DF_SFILE_SIZE, &pgctx);
    if (rval)
        printf(" failed\n");
    else
        printf(" success\n");
    
    printf("  copy test file for modification       ");
    rval = hlp_file_bytecopy(filename_o, filename_m, __TEST_DF_SFILE_SIZE, &pgctx);
    if (rval)
        printf(" failed\n");
    else
        printf(" success\n");
    
    /* modify test file copy */
    printf("\n  modify copied test file                                              ");
    rval = hlp_file_randmod(filename_m, 5, modpos, obytes, mbytes);
    if (rval)
        printf("failed\n");
    else
    {
        printf("success\n");
        for (rval = 0; rval < 5; rval++)
            printf("    Changed position %08ld: 0x%02x to 0x%02x\n", modpos[rval], obytes[rval], mbytes[rval]);
    }
    
    /* generate fingerprint */
    printf("\n  generating fingerprint of original test file                         ");
    fflush(stdout);
    rval = synctory_fingerprint(&sctx, -1, -1, filename_o, filename_fp);
    if (rval)
        printf("failed\n");
    else
        printf("success\n");
    
    printf("\n  generating fast diff from fingerprint and modified file              ");
    fflush(stdout);
    start = clock();
    rval = synctory_diff(-1, -1, -1, filename_m, filename_df, filename_fp);
    stop = clock();
    if (rval)
        printf("failed\n");
    else
        printf("success\n");
    printf("  => consumed %.2f seconds of CPU time\n", (float)(stop-start) / CLOCKS_PER_SEC);
    
    printf("\n  generating lomem diff from fingerprint and modified file             ");
    fflush(stdout);
    start = clock();
    rval = synctory_diff_lomem(-1, -1, -1, filename_m, filename_dl, filename_fp);
    stop = clock();
    if (rval)
        printf("failed\n");
    else
        printf("success\n");
    printf("  => consumed %.2f seconds of CPU time\n", (float)(stop-start) / CLOCKS_PER_SEC);
    
    printf("\n  comparing both created diff files (fast and lomem)                   ");
    fflush(stdout);
    rval = hlp_file_bincompare(filename_df, filename_dl);
    if (rval)
        printf("failed\n");
    else
        printf("success\n");
    
    if (ctx->cleanup)
    {
        unlink(filename_o);
        unlink(filename_m);
        unlink(filename_fp);
        unlink(filename_df);
        unlink(filename_dl);
    }
    
    free(filename_df);
    free(filename_dl);
    free(filename_fp);
    free(filename_m);
    free(filename_o);
    
    *status = rval;
}
