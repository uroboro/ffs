#include <stdio.h> // fprintf
#include <stdlib.h> // calloc, free
#include <string.h> // strlen
#include <ctype.h> // isdigit

#include "fsdec.h"
#include "fs_file_internal.h"

int fsReadChar(unsigned char * buffer, long fileSize, long * contentOffset, unsigned char * c) {
	if (*contentOffset >= fileSize) {
		fprintf(stderr, "Error: Out of bounds (offset(%ld) >= fileSize(%ld)).\n", *contentOffset, fileSize);
		return -1;
	}

	*c = buffer[*contentOffset];
	*contentOffset += 1;

	return 0;
}

int fsReadNumber(unsigned char * buffer, long fileSize, long * contentOffset, int * i) {
	int length = 0;
	for (;; length++) {
		long offset = *contentOffset + length;
		unsigned char c = buffer[offset];
		if (!isdigit(c)) {
			break;
		}

		if (offset >= fileSize) {
			fprintf(stderr, "Error: Going out of bounds (offset(%ld) >= fileSize(%ld)).\n", offset, fileSize);
			return -1;
		}
	}
	if (length == 0) {
		fprintf(stderr, "Error: length of number string is 0.\n");
		return -1;
	}

	unsigned char * _buffer = (unsigned char *)calloc(length + 1, sizeof(unsigned char));
	if (!_buffer) {
		fprintf(stderr, "Error: not enough memory.\n");
		return -1;
	}

	memcpy(_buffer, &(buffer[*contentOffset]), length);

	int number = 0;
	size_t readSize = sscanf((char *)_buffer, "%d", &number);
	if (readSize != 1) {
		fprintf(stderr, "Error: Couldn't read number at offset %ld.\n", *contentOffset);
		return -1;
	}
	free(_buffer);

	*contentOffset += length;
	*i = number;

	return 0;
}

int fsReadString(unsigned char * buffer, long fileSize, long * contentOffset, unsigned char ** string) {
	int r = 0;
	int nameSize = 0;
	if ((r = fsReadNumber(buffer, fileSize, contentOffset, &nameSize)) != 0) {
		return r;
	}

	if (*contentOffset >= fileSize) {
		fprintf(stderr, "Error: Out of bounds (offset(%ld) >= fileSize(%ld)).\n", *contentOffset, fileSize);
		return -1;
	}

	if (*contentOffset + nameSize >= fileSize) {
		fprintf(stderr, "Error: Going out of bounds (offset(%ld) >= fileSize(%ld)).\n", *contentOffset + nameSize, fileSize);
		return -1;
	}

	unsigned char * _buffer = (unsigned char *)calloc(nameSize + 1, sizeof(unsigned char));
	if (!_buffer) {
		fprintf(stderr, "Error: not enough memory.\n");
		return -1;
	}

	memcpy(_buffer, &(buffer[*contentOffset]), nameSize);

	*contentOffset += nameSize;
	*string = _buffer;

	return 0;
}

int fsExpectChar(unsigned char * buffer, long fileSize, long * contentOffset, unsigned char c) {
	int r = 0;
	unsigned char _buffer;
	if ((r = fsReadChar(buffer, fileSize, contentOffset, &_buffer)) != 0) {
		return r;
	}

	if (_buffer != c) {
		fprintf(stderr, "Error: Couldn't read '%c' at offset %ld, got '%c' instead.\n", c, *contentOffset, _buffer);
		return -1;
	}

	return 0;
}

int fsDeserializeHeader(unsigned char * buffer, long fileSize, long * contentOffset, fs_file * * file) {
	int r = 0;
	if ((r = fsExpectChar(buffer, fileSize, contentOffset, '{')) != 0) {
		return r;
	}

	unsigned char * name = NULL;
	if ((r = fsReadString(buffer, fileSize, contentOffset, &name)) != 0) {
		return r;
	}

	if ((r = fsExpectChar(buffer, fileSize, contentOffset, ':')) != 0) {
		return r;
	}

	int sizeOrOffset = 0;
	if ((r = fsReadNumber(buffer, fileSize, contentOffset, &sizeOrOffset)) != 0) {
		return r;
	}

	unsigned char separator = 0;
	if ((r = fsReadChar(buffer, fileSize, contentOffset, &separator)) != 0) {
		return r;
	}

	fs_file * currentFile = fsNew((char *)name, NULL);
	free(name);
	if (separator == ';') {
		currentFile->offset = sizeOrOffset;
		int count = 0;
		if ((r = fsReadNumber(buffer, fileSize, contentOffset, &count)) != 0) {
			return r;
		}

		currentFile->count = count;
		if (count > 0) {
			fs_file * * aux = (fs_file * *)calloc(count, sizeof(fs_file *));
			if (!aux) {
				fprintf(stderr, "Error: not enough memory.\n");
				return -1;
			}

			currentFile->subfiles = aux;
			for (int idx = 0; idx < count; idx++) {
				if ((r = fsDeserializeHeader(buffer, fileSize, contentOffset, &(aux[idx]))) != 0) {
					fprintf(stderr, "Error: Couldn't deserialize file %d of directory %s\n", idx, currentFile->name);
					return r;
				}
				aux[idx]->parent = currentFile;
			}
		}

		if ((r = fsExpectChar(buffer, fileSize, contentOffset, '}')) != 0) {
			return r;
		}
	} else if (separator == '}') {
		currentFile->size = sizeOrOffset;
		currentFile->count = -1;
	}

	*file = currentFile;

	return 0;
}

int fsDeserializeContents(unsigned char * buffer, long fileSize, long * contentOffset, fs_file * file) {
	if (!file) {
		fprintf(stderr, "Error: NULL file");
		return -1;
	}

	if (file->count < 0) {
		file->data = buffer + *contentOffset;
		*contentOffset += file->size;
	} else {
		for (int idx = 0; idx < file->count; idx++) {
			int r = fsDeserializeContents(buffer, fileSize, contentOffset, file->subfiles[idx]);
			if (r < 0) {
				char * subFilePath = fsFullFilePath(file->subfiles[idx]);
				fprintf(stderr, "Error: Couldn't deserialize <%s>\n", subFilePath);
				free(subFilePath);
				return -1;
			}
		}
	}

	return 0;
}

int fsDeserialize(unsigned char * buffer, long fileSize, fs_file * * file) {
	int r = 0;
	*file = NULL;

	long contentOffset = 0;
	fs_file * _file = NULL;
	if ((r = fsDeserializeHeader(buffer, fileSize, &contentOffset, &_file)) != 0) {
		fprintf(stderr, "Error: Couldn't deserialize header. Code %d\n", r);
		return r;
	}

	if ((r = fsDeserializeContents(buffer, fileSize, &contentOffset, _file)) != 0) {
		fprintf(stderr, "Error: Couldn't deserialize contents. Code %d\n", r);
		return r;
	}

	*file = _file;

	return 0;
}
