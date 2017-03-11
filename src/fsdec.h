#ifndef FSDEC_H
#define FSDEC_H

#include "fscodec.h"

fs_file * fsFilesInArchive(char * path);

int fsDeserialize(unsigned char * buffer, long fileSize, fs_file * * file);

#endif /* FSDEC_H */
