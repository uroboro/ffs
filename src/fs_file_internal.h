#ifndef FS_FILE_INTERNAL
#define FS_FILE_INTERNAL

#include "fs_file.h"

struct fs_file {
	fs_file * parent;
	char * name;
	off_t size;
	unsigned long long offset;
	union {
		struct {
			int count;
			fs_file * * subfiles;
		};
		struct {
			int unused;
			void * data;
		};
	};
};

fs_file * root_fs_file;

char * fs_popFileName(char * path, int8_t * offset);
fs_file * fs_finddir(fs_file * file, char * path);
int fs_countdir(fs_file * file);
void fs_enumerateFile(fs_file * file, void (*f)(int, fs_file *, char *));

#endif /* FS_FILE_INTERNAL */
