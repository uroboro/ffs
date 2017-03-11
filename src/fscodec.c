#include <stdio.h> // fprintf
#include <stdlib.h> // calloc, free
#include <string.h> // strlen
#include <sys/stat.h> // stat

#include "fscodec.h"
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

char * fsFullFilePath(fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return NULL;
	}

	char * path = strdup(file->name);
	if (file->parent) {
		char * subpath = NULL;
		char * parentPath = fsFullFilePath(file->parent);
		asprintf(&subpath, "%s/%s", parentPath, path);
		free(parentPath);
		free(path);
		path = subpath;
	}

	return path;
}

fs_file * fsNew(char * name, fs_file * parent) {
	fs_file * file = (fs_file *)calloc(1, sizeof(fs_file));
	if (!file) {
		fprintf(stderr, "Error: not enough memory.\n");
		return NULL;
	}

	file->name = strdup(name);
	file->parent = parent;
	char * filePath = fsFullFilePath(file);
	file->size = fsize(filePath);
	free(filePath);
	return file;
}

void fsFree(fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return;
	}

	if (file->name) {
		free(file->name);
	}
	if (file->count >= 0 && file->subfiles) {
		for (int idx = 0; idx < file->count; idx++) {
			fsFree(file->subfiles[idx]);
		}
		free(file->subfiles);
	}
	free(file);
}

int fsPrint_(fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return -1;
	}

	char * filePath = fsFullFilePath(file);
	if ((0)) {
		char * subpath = NULL;
		asprintf(&subpath, "/%s", filePath);
		free(filePath);
		filePath = subpath;
	}

	if (file->count < 0) { // definition of file
		printf("%s s:%lld\n", filePath, file->size);
		free(filePath);
		if (file->data) {
			printf("contents: \e[34m<\e[m\n");
			for (int idx = 0; idx < file->size; idx++) {
				printf("%c", ((char *)file->data)[idx]);
			}
			printf("\e[34m>\e[m\n");
		}
	} else {
		printf("%s/ c:%d o:%lld\n", filePath, file->count, file->offset);
		free(filePath);
		if (file->subfiles) {
			for (int idx = 0; idx < file->count; idx++) {
				fsPrint_(file->subfiles[idx]);
			}
		}
	}
	return 0;
}

int fsPrint(fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return -1;
	}

	return fsPrint_(file);
}
