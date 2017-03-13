#include <stdio.h> // fprintf
#include <stdlib.h> // calloc, free
#include <string.h> // strlen
#include <sys/stat.h> // stat

#include "fscodec2.h"
#include "fs_file_internal.h"

off_t fsize(const char * path) {
	struct stat st;
	if (stat(path, &st) == 0) {
		return S_ISDIR(st.st_mode) ? 0 : st.st_size;
	}

	return -1;
}

unsigned char * fsFileBufferFromFile(char * path, off_t * size) {
	if (!path || !size || *path == 0) {
		fprintf(stderr, "Error: invalid arguments.\n");
		return NULL;
	}

	*size = 0;
	off_t fileSize = fsize(path);

	FILE * fp = fopen(path, "r");
	if (!fp) {
		fprintf(stderr, "Error: Couldn't open file <%s>\n", path);
		return NULL;
	}

	unsigned char * buffer = (unsigned char *)calloc(fileSize, sizeof(unsigned char));
	if (!buffer) {
		fclose(fp);
		fprintf(stderr, "Error: not enough memory.\n");
		return NULL;
	}

	size_t readSize = fread(buffer, sizeof(unsigned char), fileSize, fp);
	fclose(fp);

	if (readSize != fileSize) {
		fprintf(stderr, "Error: Couldn't read full file <%s>\n", path);
		return NULL;
	}

	*size = fileSize;
	return buffer;
}

char endswith(char * str, char * var) {
	if (str == var) {
		return 1;
	}

	int varLen = strlen(var);
	if (varLen == 0) {
		return 0;
	}
	int strLen = strlen(str);
	if (strLen == 0) {
		return 0;
	}

	for (int i = 0; i < varLen; i++) {
		if (str[strLen - 1 - i] != var[varLen - 1 - i]) {
			return 0;
		}
	}

	return 1;
}

int filter(const struct dirent * dir) {
	return !(
		!strcmp((char *)dir->d_name, ".") ||
		!strcmp((char *)dir->d_name, "..")
	);
}
