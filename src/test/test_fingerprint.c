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
#include <unistd.h>

#include "tests.h"
#include "helpers.h"


#define __TEST_FP_SFILE_SIZE    0x7d000ULL      /* 512 KiB      */
#define __TEST_FP_LFILE_SIZE    0x1f400000ULL   /* 512 MiB      */
#define __TEST_FP_HFILE_SIZE    0xa0000000ULL   /* 2.5 GiB      */

void test_fingerprint(const test_ctx_t *ctx, int *status)
{
    char *sfile = NULL, *lfile = NULL, *hfile = NULL;
    char *sfile_fp = NULL, *lfile_fp = NULL, *hfile_fp = NULL;
    hlp_progress_t pgctx;
    size_t fnamesize;
    size_t fnamesize_fp;
    int rval;
    synctory_ctx_t sctx;
    
    fnamesize = strlen(ctx->workdir) + 15;
    fnamesize_fp = strlen(ctx->workdir) + 18;
    
    sfile = (char *)malloc(fnamesize);
    lfile = (char *)malloc(fnamesize);
    hfile = (char *)malloc(fnamesize);
    sfile_fp = (char *)malloc(fnamesize_fp);
    lfile_fp = (char *)malloc(fnamesize_fp);
    hfile_fp = (char *)malloc(fnamesize_fp);
    
    if ((NULL == sfile) || (NULL == lfile) || (NULL == hfile) || (NULL == sfile_fp) || (NULL == lfile_fp) || (NULL == hfile_fp))
    {
        *status = ENOMEM;
        free(sfile);
        free(lfile);
        free(hfile);
        free(sfile_fp);
        free(lfile_fp);
        free(hfile_fp);
        return;
    }
    
    /* prepare file names */
    hlp_path_join(ctx->workdir, "test_fp_sfile", sfile, fnamesize);
    hlp_path_join(ctx->workdir, "test_fp_lfile", lfile, fnamesize);
    hlp_path_join(ctx->workdir, "test_fp_hfile", hfile, fnamesize);
    hlp_path_join(ctx->workdir, "test_fp_sfile.fp", sfile_fp, fnamesize_fp);
    hlp_path_join(ctx->workdir, "test_fp_lfile.fp", lfile_fp, fnamesize_fp);
    hlp_path_join(ctx->workdir, "test_fp_hfile.fp", hfile_fp, fnamesize_fp);
    
    /* prepare test files */
    hlp_progress_init(&pgctx);
    pgctx.character = '.';
    pgctx.stream = stdout;
    pgctx.width = 30;
    
    printf("  preparing small test file (512 KiB)   ");
    hlp_file_bytecopy(ctx->random_device, sfile, __TEST_FP_SFILE_SIZE, &pgctx);
    printf(" done\n");
    
    printf("  preparing big test file   (512 MiB)   ");
    hlp_file_bytecopy(ctx->random_device, lfile, __TEST_FP_LFILE_SIZE, &pgctx);
    printf(" done\n");

    printf("  preparing huge test file  (2.5 GiB)   ");
    hlp_file_bytecopy(ctx->random_device, hfile, __TEST_FP_HFILE_SIZE, &pgctx);
    printf(" done\n");
    
    
    /* initialize synctory context object */
    synctory_init(&sctx);
    
    printf("\n");
    printf("  generating fingerprint of small test file                            ");
    fflush(stdout);
    rval = synctory_fingerprint(&sctx, -1, -1, sfile, sfile_fp);
    if (rval)
        printf("failed\n");
    else
        printf("success\n");
    
    printf("  generating fingerprint of big test file                              ");
    fflush(stdout);
    rval = synctory_fingerprint(&sctx, -1, -1, lfile, lfile_fp);
    if (rval)
        printf("failed\n");
    else
        printf("success\n");

    printf("  generating fingerprint of huge test file                             ");
    fflush(stdout);
    rval = synctory_fingerprint(&sctx, -1, -1, hfile, hfile_fp);
    if (rval)
        printf("failed\n");
    else
        printf("success\n");

    if (ctx->cleanup)
    {
        unlink(sfile);
        unlink(sfile_fp);
        unlink(lfile);
        unlink(lfile_fp);
        unlink(hfile);
        unlink(hfile_fp);
    }
    
    free(sfile);
    free(lfile);
    free(hfile);
    free(sfile_fp);
    free(lfile_fp);
    free(hfile_fp);
    *status = 0;
}
