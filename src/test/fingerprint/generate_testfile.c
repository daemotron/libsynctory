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


/*
	generate_testfile writes into the specified file 4 chunks of random bytes,
	each chunk the length of SYNCTORY_CHUNK_BYTES bytes.
	
	The first and the third chunk are identical, as are the second and the 
	fourth one. This makes it possible to verify that identical chunks
	generate identical fingerprints.
*/

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "_fingerprint.h"

void usage(void)
{
	printf("Usage: generate_testfile DESTINATION\n");
}



int main(int argc, char **argv)
{
	char cwd[1024] = { '\0' };
	unsigned char rbuf1[SYNCTORY_CHUNK_BYTES];
	unsigned char rbuf2[SYNCTORY_CHUNK_BYTES];
	char *destfile = NULL;
	int fd;
	unsigned int i;
	ssize_t bytes;
	
	/*
	Program has to be called with one argument. Argument is 
	the target file for the test file content.
	The file has to be writeable or must be located in a
	writeable path.
	
	Relative paths are possible; they're converted into absolute
	paths with the following code.
	*/
	if (argc > 1)
	{
		if ((NULL != argv[1]) && (strlen(argv[1]) > 0))
		{
			if (argv[1][0] != '/')
			{
				memset(cwd, (int)'\0', 1024);
				getcwd(cwd, 1024);
				destfile = (char *)malloc(strlen(cwd) + strlen(argv[1]) + 2);
				strncpy(destfile, cwd, strlen(cwd) + 1);
				strncat(destfile, "/", 2);
				strncat(destfile, argv[1], strlen(argv[1]) + 1);
			}
			else
			{
				destfile = (char *)malloc(strlen(cwd) + strlen(argv[1]) + 2);
				strncpy(destfile, argv[1], strlen(argv[1]) + 1);
			}
		}
		else
		{
			printf("Argument is invalid: %s\n", argv[1]);
			return EXIT_FAILURE;
		}
	}
	else
	{
		usage();
		return EXIT_FAILURE;
	}
	
	/* initialize random number generator */
	srand(time(NULL));
	
	/* fill the two buffers with random stuff */
	for (i = 0; i < SYNCTORY_CHUNK_BYTES; ++i)
	{
		rbuf1[i] = (unsigned char)(rand() % 256);
		rbuf2[i] = (unsigned char)(rand() % 256);
	}
	
	/* open the destination file */
	fd = open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	
	for (i = 0; i < 2; ++i)
	{
		bytes = write(fd, &rbuf1[0], SYNCTORY_CHUNK_BYTES);
		if (bytes != SYNCTORY_CHUNK_BYTES)
		{
			fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
			close(fd);
			return EXIT_FAILURE;
		}
	
		bytes = write(fd, &rbuf2[0], SYNCTORY_CHUNK_BYTES);
		if (bytes != SYNCTORY_CHUNK_BYTES)
		{
			fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
			close(fd);
			return EXIT_FAILURE;
		}
	}
	
	close(fd);
	return EXIT_SUCCESS;
}
