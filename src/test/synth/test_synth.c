/**
 * Copyright (c) 2010 Jesco Freund.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Backtory nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  $Id$
 */


#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "_synth.h"

void usage(void)
{
	printf("Usage: test_synth SOURCE DIFF DESTINATION\n");
}


int main(int argc, char **argv)
{
	char cwd[1024] = { '\0' };
	char *diffile = NULL;
	char *sourcefile = NULL;
	char *destfile = NULL;
	int rval = 0;
	
	if (argc > 1)
	{
		if ((NULL != argv[1]) && (strlen(argv[1]) > 0))
		{
			if (argv[1][0] != '/')
			{
				memset(cwd, (int)'\0', 1024);
				getcwd(cwd, 1024);
				sourcefile = (char *)malloc(strlen(cwd) + strlen(argv[1]) + 2);
				strncpy(sourcefile, cwd, strlen(cwd) + 1);
				strncat(sourcefile, "/", 2);
				strncat(sourcefile, argv[1], strlen(argv[1]) + 1);
			}
			else
			{
				sourcefile = (char *)malloc(strlen(cwd) + strlen(argv[1]) + 2);
				strncpy(sourcefile, argv[1], strlen(argv[1]) + 1);
			}
		}
		else
		{
			printf("First argument is invalid: %s\n", argv[1]);
			return EXIT_FAILURE;
		}
	}
	else
	{
		usage();
		return EXIT_FAILURE;
	}
	
	if (argc > 2)
	{
		if ((NULL != argv[2]) && (strlen(argv[2]) > 0))
		{
			if (argv[2][0] != '/')
			{
				memset(cwd, (int)'\0', 1024);
				getcwd(cwd, 1024);
				diffile = (char *)malloc(strlen(cwd) + strlen(argv[2]) + 2);
				strncpy(diffile, cwd, strlen(cwd) + 1);
				strncat(diffile, "/", 2);
				strncat(diffile, argv[2], strlen(argv[2]) + 1);
			}
			else
			{
				diffile = (char *)malloc(strlen(cwd) + strlen(argv[2]) + 2);
				strncpy(diffile, argv[2], strlen(argv[2]) + 1);
			}
		}
		else
		{
			printf("Second argument is invalid: %s\n", argv[2]);
			return EXIT_FAILURE;
		}
	}
	else
	{
		usage();
		return EXIT_FAILURE;
	}
	
	if (argc > 3)
	{
		if ((NULL != argv[3]) && (strlen(argv[3]) > 0))
		{
			if (argv[3][0] != '/')
			{
				memset(cwd, (int)'\0', 1024);
				getcwd(cwd, 1024);
				destfile = (char *)malloc(strlen(cwd) + strlen(argv[3]) + 2);
				strncpy(destfile, cwd, strlen(cwd) + 1);
				strncat(destfile, "/", 2);
				strncat(destfile, argv[3], strlen(argv[3]) + 1);
			}
			else
			{
				destfile = (char *)malloc(strlen(cwd) + strlen(argv[3]) + 2);
				strncpy(destfile, argv[3], strlen(argv[3]) + 1);
			}
		}
		else
		{
			printf("First argument is invalid: %s\n", argv[3]);
			return EXIT_FAILURE;
		}
	}
	else
	{
		usage();
		return EXIT_FAILURE;
	}
	
	rval = synctory_synth_create_fn(sourcefile, diffile, destfile);
	
	if(rval)
		printf("Error %d: %s\n", rval, (rval < 0 ? strerror(errno) : strerror(rval)));
	
	return EXIT_SUCCESS;
}
