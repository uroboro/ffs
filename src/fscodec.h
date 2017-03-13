#ifndef FSCODEC_H
#define FSCODEC_H

#include "fs_file.h"

char * fsFullFilePath(fs_file * file);
fs_file * fsNew(char * name, fs_file * parent);
void fsFree(fs_file * file);

int fsPrint(fs_file * file);

#endif /* FSCODEC_H */
