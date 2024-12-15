#include <stdio.h>

#ifndef S21_CAT_H

typedef struct {
    int b, e, v, t, n, s;
} flags_t;

flags_t inputFlags(int, char**);

void readFile(char**, const flags_t*);

void printLine(const flags_t*, FILE*);

void currentLine(unsigned char*, unsigned long, const flags_t*);

int getLine(unsigned char**, unsigned long*, FILE*);

#endif