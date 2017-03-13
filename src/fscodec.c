#include <stdio.h> // fprintf
#include <stdlib.h> // calloc, free
#include <string.h> // strlen
#include <sys/stat.h> // stat

#include "fscodec.h"
#include "fs_file_internal.h"

char * fsFullFilePath(fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return NULL;
	}

	char * path = strdup(file->name);
	if (!file->parent) {
		return path;
	}

	char * subpath = NULL;
	char * parentPath = fsFullFilePath(file->parent);
	asprintf(&subpath, "%s/%s", parentPath, path);
	free(parentPath);
	free(path);

	return subpath;
}

fs_file * fsNew(char * name, fs_file * parent) {
	fs_file * file = (fs_file *)calloc(1, sizeof(fs_file));
	if (!file) {
		fprintf(stderr, "Error: not enough memory.\n");
		return NULL;
	}

	file->name = strdup(name);
	file->parent = parent;
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
