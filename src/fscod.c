#include <stdio.h> // fprintf
#include <stdlib.h> // calloc, free
#include <string.h> // strlen
#include <dirent.h> // struct dirent

#include "fscod.h"
#include "fscodec2.h" // fsize
#include "fs_file_internal.h"

int fsFilesAtPath_(fs_file * file, unsigned long long * offset) {
	char * filePath = fsFullFilePath(file);

	struct dirent * * eps;
	int n = scandir(filePath, &eps, &filter, alphasort);
	if (n < 0) { // filePath is a file
		*offset += file->size;
		return -1;
	}
	free(filePath);

	file->offset = *offset;

	fs_file * * aux = (fs_file * *)calloc(n, sizeof(fs_file *));
	if (!aux) {
		fprintf(stderr, "Error: not enough memory.\n");
		return -1;
	}
	file->subfiles = aux;

	for (int idx = 0; idx < n; idx++) {
		char * name = eps[idx]->d_name;
		aux[idx] = fsNew(name, file);
		{
			char * filePath = fsFullFilePath(aux[idx]);
			aux[idx]->size = fsize(filePath);
			free(filePath);
		}
		aux[idx]->count = fsFilesAtPath_(aux[idx], offset);
	}

	return n;
}

fs_file * fsFilesAtPath(char * path) {
	unsigned long long offset = 0;
	fs_file * file = fsNew(path, NULL);
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return NULL;
	}
	{
		char * filePath = fsFullFilePath(file);
		file->size = fsize(filePath);
		free(filePath);
	}
	file->count = fsFilesAtPath_(file, &offset);
	return file;
}

int fsSerializeHeader_(FILE * fp, fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return -1;
	}

	fprintf(fp, "{%lu%s", strlen(file->name), file->name);
	if (file->count >= 0) {
		fprintf(fp, ":%lld;%d", file->offset, file->count);
		for (int idx = 0; idx < file->count; idx++) {
			fsSerializeHeader_(fp, file->subfiles[idx]);
		}
	} else {
		fprintf(fp, ":%lld", file->size);
	}
	fprintf(fp, "}");
	return 0;
}

int fsSerializeHeader(FILE * fp, fs_file * file) {
	fs_file * root = fsNew("", NULL);
	fs_file * * aux = (fs_file * *)calloc(1, sizeof(fs_file *));
	if (!aux) {
		fprintf(stderr, "Error: not enough memory.\n");
		return -1;
	}
	root->subfiles = aux;
	root->count = 1;

	root->subfiles[0] = file;

	return fsSerializeHeader_(fp, root);
}

int fsSerializeContents(FILE * fp, fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return -1;
	}

	char * filePath = fsFullFilePath(file);

	if (file->count >= 0) {
		for (int idx = 0; idx < file->count; idx++) {
			int r = fsSerializeContents(fp, file->subfiles[idx]);
			if (r < 0) {
				char * subFilePath = fsFullFilePath(file->subfiles[idx]);
				fprintf(stderr, "Error: Couldn't serialize <%s>\n", subFilePath);
				free(subFilePath);
				return -1;
			}
		}
	} else {
		FILE * content_fp = fopen(filePath, "r");
		unsigned char * ptr = (unsigned char *)malloc(file->size * sizeof(unsigned char));
		size_t readSize = fread(ptr, sizeof(char), file->size, content_fp);
		if (readSize != file->size) {
			fprintf(stderr, "Error: Couldn't read full file <%s>\n", filePath);
			return -1;
		}
		size_t writeSize = fwrite(ptr, sizeof(char), file->size, fp);
		if (writeSize != file->size) {
			fprintf(stderr, "Error: Couldn't write full file <%s>\n", filePath);
			return -2;
		}
		fclose(content_fp);
		free(ptr);
	}

	free(filePath);
	return 0;
}

int fsSerialize(char * path, fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file\n");
		return -1;
	}

	int r = 0;
	FILE * fp = fopen(path, "w");
	if ((r = fsSerializeHeader(fp, file)) != 0) {
		return r;
	}
	if ((r = fsSerializeContents(fp, file)) != 0) {
		return r;
	}
	fclose(fp);
	return r;
}
