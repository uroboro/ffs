#ifndef FSCOD_H
#define FSCOD_H

#include "fscodec.h"

fs_file * fsFilesAtPath(char * path);

int fsSerialize(char * path, fs_file * file);

#endif /* FSCOD_H */
