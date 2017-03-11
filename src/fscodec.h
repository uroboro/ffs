#ifndef FSCODEC_H
#define FSCODEC_H

#include <dirent.h> // struct dirent

off_t fsize(const char * filename);
unsigned char * fsFileBufferFromFile(char * path, off_t * size);
char endswith(char * str, char * var);
int filter(const struct dirent * dir);

#include "fs_file.h"

char * fsFullFilePath(fs_file * file);
fs_file * fsNew(char * name, fs_file * parent);
void fsFree(fs_file * file);

int fsPrint(fs_file * file);

#endif /* FSCODEC_H */
