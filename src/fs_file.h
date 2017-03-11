#ifndef FS_FILE
#define FS_FILE

#include "replacements.h"

typedef struct fs_file fs_file;

fs_file * fs_fopen(char * filePath, char * mode);
void fs_fclose(fs_file * file);
uint32_t fs_read(fs_file * file, uint32_t offset, uint32_t size, uint8_t * buffer);

#endif /* FS_FILE */
