#include <stdio.h> // *printf, stderr
#include <string.h> // strlen
#include <stdlib.h> // calloc, free

#include "fsdec.h"
#include "fs_file_internal.h"

fs_file * root_fs_file = NULL;

char * fs_popFileName(char * path, int8_t * offset) {
	if (!path || !offset) {
		return NULL;
	}
	int length = strlen(path);
	if (!length) {
		return NULL;
	}

	int8_t offset_ = 0;
	char escaped = 0, keepGoing = 1;
	for (int i = 0; keepGoing && i < length; i++) {
		switch (path[i]) {
			case '\\':
				escaped = 1;
				break;

			case '/':
				if (escaped) {
					escaped = 0;
					offset_++;
				} else {
					keepGoing = 0;
				}
				break;

			default:
				offset_++;
				break;
		}
	}

	char * pathComponent = (char *)calloc(offset_ + 1, sizeof(char));
	memcpy(pathComponent, path, offset_);
	if (!path[offset_]) {
		*offset = -1;
	} else {
		*offset = offset_ + 1;
	}
	return pathComponent;
}

fs_file * fs_fopen(char * filePath, char * mode) {
	mode = NULL;
	fs_file * file = fs_finddir(NULL, filePath);
	if (!file) {
		fprintf(stderr, "Couldn't find file %s.\n", filePath);
		return NULL;
	}
	return file;
}

void fs_close(fs_file * node) {
	node = NULL;
}

uint32_t fs_read(fs_file * file, uint32_t offset, uint32_t size, uint8_t * buffer) {
	if (!buffer) {
		return 0;
	}

	if (offset >= file->size) {
		*buffer = EOF;
		return 0;
	}
	if (offset + size > file->size) {
		size = file->size - offset;
	}
	memcpy(buffer, (uint8_t *)(file->data + offset), size);
	return size;
}

fs_file * fs_finddir(fs_file * file, char * path) {
	fs_file * r = NULL;

	int8_t offset = 0;
	char * name = fs_popFileName(path, &offset);

	if (offset < 0) { // last component
		int count = fs_countdir(file);
		for (int idx = 0; idx < count; idx++) {
			fs_file * subfile = file->subfiles[idx];
			if (!strcmp(name, subfile->name)) {
				r = subfile;
				break;
			}
		}
	} else {
		if (!*name) {
			r = fs_finddir(root_fs_file, &path[offset]);
		} else {
			int count = fs_countdir(file);
			for (int idx = 0; idx < count; idx++) {
				fs_file * subfile = file->subfiles[idx];
				if (!strcmp(name, subfile->name)) {
					if (subfile->count < 0) {
						r = subfile;
						break;
					} else {
						r = fs_finddir(file->subfiles[idx], &path[offset]);
						break;
					}
				}
			}
		}
	}

	free(name);

	return r;
}

int fs_countdir(fs_file * file) {
	if (!file) {
		return -1;
	}

	return file->count;
}

void fs_enumerateFile(fs_file * file, void (*f)(int, fs_file *, char *)) {
	if (!file) {
		fprintf(stderr, "Error: no file\n");
	}

	int count = fs_countdir(file);
	char stop = 0;
	for (int idx = 0; idx < count && !stop; idx++) {
		if (f) {
			f(idx, file->subfiles[idx], &stop);
		}
	}
}

fs_file * fsFilesAtLocation(uint32_t location, uint32_t length) {
	int r = 0;
	unsigned char * buffer = (unsigned char *)(unsigned long)location;
	long fileSize = (long)length;

	fs_file * file = NULL;
	if ((r = fsDeserialize(buffer, fileSize, &file)) != 0) {
		fprintf(stderr, "Error: Couldn't deserialize buffer. Code %d\n", r);
		return NULL;
	}

	return file;
}
