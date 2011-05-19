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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <openssl/ripemd.h>

#include "_checksum.h"

/* lenth of the hexdigest result buffer */
#define HEX_LEN (RIPEMD160_DIGEST_LENGTH * 2 + 1)

int main(void)
{
	int i, j;
	char *ts[] = { 
		"A",
		"AA",
		"AAA",
		"AAAA",
		"AAAAA",
		"AAAAAA",
		"AAAAAAA",
		"AAAAAAAA",
		"AAAAAAAAAAAAAAAA",
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" 
	};
	char *t2 = "Hello World";
	char *t3 = "ello Worlda";
	uint32_t result;
	synctory_checksum_t weaksum, weaksum2;
	
	synctory_checksum_init(&weaksum);
	synctory_checksum_init(&weaksum2);
	
	unsigned char md[SYNCTORY_STRONG_CHECKSUM_BYTES] = { '\0' };
	char rbuf[HEX_LEN] = { '\0' };
	
	/* Test 1: "Hello World" */
	printf("Test %2d: %s\n-------------------------------------------------------------\n", 1, t2);
	
	/* generate weak checksum for test string  and print it to stdout */
	result = synctory_weak_checksum(t2, strlen(t2));
	printf("Weak checksum:   0x%08x\n", result);
	
	/* generate strong checksum from test string. reformat the result as 
	   human-readable hex number and print it to stdout */
	synctory_strong_checksum(t2, strlen(t2), &md[0], SYNCTORY_CHECKSUM_DEFAULT);
	for (j = 0; j < SYNCTORY_STRONG_CHECKSUM_BYTES; ++j)
		snprintf(&rbuf[j * 2], 3, "%02x", md[j]);
	printf("Strong checksum: 0x%s\n", rbuf);
	printf("\n");
	
	/* Do the same as above for the AAAA... buffers */
	for (i = 0; i < 10; ++i)
	{
		printf("Test %2d: %s (%d bytes)\n-------------------------------------------------------------\n", i+2, ts[i], (int)strlen(ts[i]));
		result = synctory_weak_checksum(ts[i], strlen(ts[i]));
		printf("Weak checksum:   0x%08x\n", result);
		synctory_strong_checksum(ts[i], strlen(ts[i]), &md[0], SYNCTORY_CHECKSUM_DEFAULT);
		for (j = 0; j < SYNCTORY_STRONG_CHECKSUM_BYTES; ++j)
			snprintf(&rbuf[j * 2], 3, "%02x", md[j]);
		printf("Strong checksum: 0x%s\n", rbuf);
		printf("\n");
	}
	
	printf("\nWEAK CHECKSUM TEST\n==================\n");
	synctory_checksum_update(&weaksum, t2, strlen(t2));
	printf("Test string: \"%s\" (length: %d)\n", t2, (int)strlen(t2));
	printf("Calculated digest: 0x%08x\n\n", synctory_checksum_digest(&weaksum));
	synctory_checksum_update(&weaksum2, t3, strlen(t3));
	synctory_checksum_rotate(&weaksum, 'H', 'a');
	printf("Rotated string: \"%s\" (length: %d)\n", t3, (int)strlen(t3));
	printf("Calculated digest: 0x%08x\n", synctory_checksum_digest(&weaksum2));
	printf("Rotated digest:    0x%08x\n", synctory_checksum_digest(&weaksum));
	
	return EXIT_SUCCESS;
}

