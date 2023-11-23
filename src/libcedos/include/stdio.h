#ifndef STDIO_H
#define STDIO_H
#include <stdarg.h>
#include <stddef.h>

typedef struct { int unused; } FILE;

#define stdin (FILE*)(0)
#define stdout (FILE*)(1)
#define stderr (FILE*)(1)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define EOF (-1)

FILE* fopen(const char*, const char*);

int fputc ( int character, FILE * stream );
int fputs ( const char * str, FILE * stream );

int fgetc ( FILE * stream );
char * fgets ( char * str, int num, FILE * stream );

size_t fread(void*, size_t, size_t, FILE*);
size_t fwrite(const void*, size_t, size_t, FILE*);

int sprintf ( char * str, const char * format, ... );
int fprintf(FILE*, const char*, ...);
int printf(const char *fmt, ...);

int fseek(FILE*, long, int);
long ftell(FILE*);

#endif