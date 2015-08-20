/*
	Code written 100% by Valentino Giudice.
	All rights reserved.
*/

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

void sceda_digest(unsigned char*, const unsigned char*, int);

int main() {
	FILE *fp1, *fp2;
	char word[101], hexaDigest[33];
	unsigned char digest[16];
	int i;

	fp1 = fopen("wordlist.txt", "r");
	fp2 = fopen("output.csv", "w");
	while(fgets(word, 100, fp1) != NULL) {
		word[strlen(word) - 1] = '\0';
		sceda_digest(digest, word, strlen(word));
		for(i = 0; i < 16; i++) {
			sprintf(hexaDigest + i * 2, "%02x", digest[i]);
		}
		fprintf(fp2, "\"%s\"\t\"%s\"\n", word, hexaDigest);
	}

	fclose(fp1);
	fclose(fp2);
	return 0;
}

#define T1(x)	(((~((x) >> 16)) << 16) | ((x) & 0xFFFF))
#define T2(x)	(~(T1(x)))
#define MV 8191	/* Other possible values are 3, 7, 31, 127, 8191, 131071, 524287 and 2147483647. */
#define M(x, y)	((((x) * (y)) ^ (((x) + MV) * ((y) + MV))))

void sceda_digest_func(unsigned char *output, const unsigned char *input) {
	unsigned char buffer[16];
	unsigned int k, a, b, c, d, temp, tempA, i;

	memcpy(buffer, input, 16);
	for(i = 0; i < 4; i++) {
		a = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
		b = (buffer[4] << 24) + (buffer[5] << 16) + (buffer[6] << 8) + buffer[7];
		c = (buffer[8] << 24) + (buffer[9] << 16) + (buffer[10] << 8) + buffer[11];
		d = (buffer[12] << 24) + (buffer[13] << 16) + (buffer[14] << 8) + buffer[15];
		k = b ^ c ^ d;
		temp = a;
		tempA = a;
		a = M(T1(a), T2(b)) ^ k;
		k = temp ^ a;
		temp = b;
		b = M(T1(b), T2(c)) ^ k;
		k = temp ^ b;
		temp = c;
		c = M(T1(c), T2(d)) ^ k;
		k = temp ^ c;
		d = M(T1(d), T2(tempA)) ^ k;
		buffer[1] = a >> 24;
		buffer[2] = a >> 16;
		buffer[3] = a >> 8;
		buffer[4] = a;
		buffer[5] = c >> 24;
		buffer[6] = c >> 16;
		buffer[7] = c >> 8;
		buffer[8] = c;
		buffer[9] = b >> 24;
		buffer[10] = b >> 16;
		buffer[11] = b >> 8;
		buffer[12] = b;
		buffer[13] = d >> 24;
		buffer[14] = d >> 16;
		buffer[15] = d >> 8;
		buffer[0] = d;
	}
	memcpy(output, buffer, 16);
}

void sceda_digest(unsigned char *output, const unsigned char *input, int length) {
	unsigned char *buffer, temp1[16], temp2[16];
	int blockC, i, j;

	blockC = (length + 17) / 16;
	buffer = (unsigned char*)malloc(blockC * 16);
	memcpy(buffer, input, length);
	buffer[length] = length / 256;
	buffer[length + 1] = length % 256;
	for(i = length + 1; i < blockC * 16; i++) {
		buffer[i] = 170;
	}
	for(i = 0; i < blockC; i++) {
		sceda_digest_func(buffer + 16 * i, buffer + 16 * i);
	}
	memcpy(output, buffer, 16);
	for(i = 1; i < blockC; i++) {
		memcpy(temp1, output, 8);
		memcpy(temp1 + 8, buffer + 16 * i, 8);
		memcpy(temp2, buffer + 16 * i + 8, 8);
		memcpy(temp2 + 8, output + 8, 8);
		sceda_digest_func(temp1, temp1);
		sceda_digest_func(temp2, temp2);
		for(j = 0; j < 16; j++) {
			output[j] = temp1[j] ^ temp2[j];
		}
	}

	free(buffer);
}
