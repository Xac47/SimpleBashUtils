#include <regex.h>

#ifndef S21_GREP_H

typedef struct {
    int e, i, c, v, l, n, h, s, o, f, lenPattern;
    char pattern[1024];
} flags_t;

void patternAdd(flags_t*, char*);

void addRegFromFile(flags_t*, char*);

flags_t inputFlags(int, char**);

void outputLine(char*, int);

void printMath(regex_t*, char*);

void printFile(flags_t, char*, regex_t*);

void readFile(flags_t, int, char**);

#endif
