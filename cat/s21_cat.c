#include "s21_cat.h"

#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    flags_t flags = inputFlags(argc, argv);

    readFile(argv, &flags);

    return 0;
}

flags_t inputFlags(int argc, char** argv) {
    flags_t flags = {0};

    struct option long_options[] = {
        {"number-nonblank", 0, NULL, 'b'},
        {"number", 0, NULL, 'n'},
        {"squeeze-blank", 0, NULL, 's'},
    };

    int opt = 0;

    while ((opt = getopt_long(argc, argv, "bevtnsET", long_options, NULL)) != -1) {
        switch (opt) {
            case 'b':
                flags.b = 1;
                flags.n = 0;
                break;
            case 'e':
                flags.e = 1;
                flags.v = 1;
                break;
            case 'v':
                flags.v = 1;
                break;
            case 'n':
                flags.n = !flags.b;
                break;
            case 's':
                flags.s = 1;
                break;
            case 't':
                flags.t = 1;
                flags.v = 1;
                break;
            case 'T':
                flags.t = 1;
                break;
            case 'E':
                flags.e = 1;
                break;
            case '?':
                fprintf(stderr, "usage: cat [-benstvTE] [file ...]");
                exit(1);
                break;
        }
    }
    return flags;
}

void readFile(char** argv, const flags_t* flags) {
    FILE* fp;
    fp = fopen(argv[optind], "r");
    if (fp == NULL) {
        fprintf(stderr, "cat: %s: No such file or directory\n", argv[optind]);
    }
    printLine(flags, fp);
    fclose(fp);
}

void printLine(const flags_t* flags, FILE* fp) {
    size_t len = 0;
    unsigned char* line = NULL;
    int numberLine = 1, passLine = 0;

    while (getLine(&line, &len, fp) != -1) {
        if (flags->s && line[0] == '\n' && passLine) continue;
        if (flags->n || (flags->b && line[0] != '\n')) {
            printf("%6d\t", numberLine);
            numberLine++;
        }
        currentLine(line, len, flags);

        if (line[0] == '\n')
            passLine = 1;
        else
            passLine = 0;
    }
    free(line);
    fflush(stdout);
}

void currentLine(unsigned char* line, unsigned long len, const flags_t* flags) {
    for (unsigned long i = 0; i < len; ++i) {
        if (flags->e && line[i] == '\n') printf("$");
        if (line[i] == '\t' && flags->t) {
            printf("^I");
        } else if (flags->v && !isprint(line[i]) && line[i] != '\n' && line[i] != '\t') {
            unsigned char ch = (unsigned char)line[i];
            if (ch == 127)
                printf("^?");
            else if (ch == 255)
                printf("M-^?");
            else if (iscntrl(ch))
                printf("^%c", ch + 64);
            else if (ch > 127 && ch < 160)
                printf("M-^%c", ch - 64);
            else if (ch > 159)
                printf("M-%c", ch - 128);
            else
                printf("%c", ch);

        } else
            printf("%c", line[i]);
    }
}

int getLine(unsigned char** line, unsigned long* len, FILE* fp) {
    unsigned char c;
    unsigned long capacity = 4, size = 0;

    if (*line != NULL) free(*line);
    *line = (unsigned char*)malloc(capacity * sizeof(unsigned char));

    while ((c = fgetc(fp)) != '\n' && !feof(fp) && !ferror(fp)) {
        if (size + 2 >= capacity) {
            capacity *= 2;
            unsigned char* tmp = malloc(capacity * sizeof(unsigned char));

            for (unsigned long i = 0; i < size; ++i) {
                tmp[i] = (*line)[i];
            }
            free(line);
            *line = tmp;
        }
        (*line)[size++] = c;
    }
    if (c == '\n') {
        (*line)[size++] = c;
    }
    int result = 0;
    if (size == 0 && (feof(fp) || ferror(fp))) {
        result = -1;
    }

    *len = size;
    return result;
}
