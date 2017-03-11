#include <stdio.h> // *printf, stderr
#include <string.h> // strlen
#include <stdlib.h> // calloc, free

#include "rd_file_internal.h"
#include "replacements.h"

RD_FILE * rd_fopen(char * filePath, char * mode) {
	fs_file * file = fs_fopen(filePath, mode);
	if (!file) {
		fprintf(stderr, "ERROR: couldn't open file.\n");
		return NULL;
	}

	RD_FILE * stream = (RD_FILE *)calloc(1, sizeof(RD_FILE));
	if (!stream) {
		fprintf(stderr, "ERROR: not enough memory.\n");
		return NULL;
	}
	stream->file = file;
	stream->fpos = 0;
	return stream;
}

void rd_fclose(RD_FILE * stream) {
	stream->file = NULL;
	stream->fpos = 0;
	free(stream);
}

size_t rd_fread(void * ptr, size_t size, size_t nmemb, RD_FILE * stream) {
	unsigned char * chbuf = (unsigned char *)ptr;
	uint32_t sum = 0;
	for (size_t i = 0; i < nmemb; i++) {
		uint8_t buf;
		sum += fs_read(stream->file, stream->fpos++, size, &buf);
		if ((int8_t)buf == EOF) {
			return EOF;
		}
		chbuf[i] = buf;
	}
	return sum;
}

int rd_fgetc(RD_FILE * stream) {
	uint8_t ch;
	if (!fs_read(stream->file, stream->fpos++, 1, &ch) || (int8_t)ch == EOF) {
		return EOF;
	}
	return ch;
}

char * rd_fgets(char * s, int size, RD_FILE * stream) {
	char * cs = s;
	int c;
	while (--size > 0 && (c = rd_fgetc(stream)) != EOF) {
		if ((*cs++ = c) == '\n') {
			break;
		}
	}
	*cs = '\0';
	return (c == EOF && cs == s) ? (char *)EOF : s;
}
