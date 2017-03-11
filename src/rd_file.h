#ifndef RD_FILE_H
#define RD_FILE_H

typedef struct file_t RD_FILE;

RD_FILE * rd_fopen(char * filename, char * mode);
void rd_fclose(RD_FILE * stream);
size_t rd_fread(void * ptr, size_t size, size_t nmemb, RD_FILE * stream);

int rd_fgetc(RD_FILE * stream);
char * rd_fgets(char * s, int size, RD_FILE * stream);

#endif /* RD_FILE_H */
