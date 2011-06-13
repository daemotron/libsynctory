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

#include "tests.h"
#include "helpers.h"


#define __TEST_FP_SFILE_SIZE    0x7d000ULL      /* 512 KiB      */
#define __TEST_FP_LFILE_SIZE    0x1f400000ULL   /* 512 MiB      */
#define __TEST_FP_HFILE_SIZE    0xa0000000ULL   /* 2.5 GiB      */

void test_fingerprint(const test_ctx_t *ctx, int *status)
{
    char *sfile = NULL, *lfile = NULL, *hfile = NULL;
    hlp_progress_t pgctx;
    size_t fnamesize;
    
    fnamesize = strlen(ctx->workdir) + 15;
    
    sfile = (char *)malloc(fnamesize);
    lfile = (char *)malloc(fnamesize);
    hfile = (char *)malloc(fnamesize);
    
    if ((NULL == sfile) || (NULL == lfile) || (NULL == hfile))
    {
        *status = ENOMEM;
        free(sfile);
        free(lfile);
        free(hfile);
        return;
    }
    
    
    /* prepare file names */
    hlp_path_join(ctx->workdir, "test_fp_sfile", sfile, fnamesize);
    hlp_path_join(ctx->workdir, "test_fp_lfile", lfile, fnamesize);
    hlp_path_join(ctx->workdir, "test_fp_hfile", hfile, fnamesize);
    
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

    free(sfile);
    free(lfile);
    free(hfile);
    *status = 0;
}
