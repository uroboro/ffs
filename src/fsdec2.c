#include <stdio.h> // fprintf
#include <stdlib.h> // calloc, free
#include <string.h> // strlen
#include <ctype.h> // isdigit

#include "fsdec.h"
#include "fsdec2.h"
#include "fscodec2.h"
#include "fs_file_internal.h"

fs_file * fsFilesInArchive(char * path) {
	int r = 0;

	off_t fileSize = fsize(path);
	unsigned char * buffer = fsFileBufferFromFile(path, &fileSize);
	if (!buffer) {
		fprintf(stderr, "Error: Couldn't read full file <%s>\n", path);
		return NULL;
	}

	fs_file * file = NULL;
	if ((r = fsDeserialize(buffer, fileSize, &file)) != 0) {
		fprintf(stderr, "Error: Couldn't deserialize buffer. Code %d\n", r);
		return NULL;
	}

	return file;
}

__attribute__((constructor))
int ctor(int argc, char * * argv) {
	char * path = (argc > 1) ? argv[1] : NULL;
	if (!path) {
		return 0;
	}

	while (endswith(path, "/")) {
		path[strlen(path) - 1] = 0;
	}
	root_fs_file = fsFilesInArchive(path);

	return 0;
}
