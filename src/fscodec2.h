#ifndef FSCODEC2_H
#define FSCODEC2_H

#include <dirent.h> // struct dirent

off_t fsize(const char * filename);
unsigned char * fsFileBufferFromFile(char * path, off_t * size);
char endswith(char * str, char * var);
int filter(const struct dirent * dir);

#endif /* FSCODEC2H */
