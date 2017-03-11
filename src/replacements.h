#ifndef FS_H
#define FS_H

#ifndef EOF
#define EOF (-1)
#endif

//#include <sys/types.h>
#ifndef __TYPES_H_
typedef char int8_t;
//typedef uint16_t mode_t;
#endif

//#include <stdint.h>
#ifndef _STDINT_H_
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef int int32_t;
#endif

typedef unsigned long size_t;

//#include <stdlib.h>
void * malloc(unsigned long);
#define kmalloc(...) malloc(__VA_ARGS__)

//#include <stdio.h>
#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR	 0x0002

#ifndef NULL
#define NULL (void *)0
#endif
#define printf_err(...)

//#include <string.h>
#ifndef _STRING_H_
void * memset(void *, int, unsigned long);
void * memcpy(void *, const void *, unsigned long);
char * strcpy(char *, const char *);
unsigned long strlen(const char *);
int strcmp(const char *, const char *);
#endif

#endif
