#include "s21_grep.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    flags_t flags = inputFlags(argc, argv);
    readFile(flags, argc, argv);
    return 0;
}

flags_t inputFlags(int argc, char **argv) {
    flags_t flags = {0};
    int opt;

    while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
        switch (opt) {
            case 'e':
                flags.e = 1;
                patternAdd(&flags, optarg);
                break;
            case 'i':
                flags.i = REG_ICASE;
                break;
            case 'v':
                flags.v = 1;
                break;
            case 'c':
                flags.c = 1;
                break;
            case 'l':
                flags.l = 1;
                break;
            case 'n':
                flags.n = 1;
                break;
            case 'h':
                flags.h = 1;
                break;
            case 'f':
                flags.f = 1;
                addRegFromFile(&flags, optarg);
                break;
            case 'o':
                flags.o = 1;
                break;
            default:;
                break;
        }
    }
    if (flags.lenPattern == 0) {
        patternAdd(&flags, argv[optind]);
        optind++;
    }
    if (argc - optind == 1) {
        flags.h = 1;
    }
    return flags;
}

void readFile(flags_t flags, int argc, char **argv) {
    regex_t re;
    int error = regcomp(&re, flags.pattern, REG_EXTENDED | flags.i);
    if (error) {
        perror("ERROR");
        exit(1);
    }

    for (int i = optind; i < argc; ++i) {
        printFile(flags, argv[i], &re);
    }
    regfree(&re);
}

void outputLine(char *line, int n) {
    for (int i = 0; i < n; ++i) {
        putchar(line[i]);
    }
    if (line[n - 1] != '\n') putchar('\n');
}

void patternAdd(flags_t *flags, char *pattern) {
    if (flags->lenPattern != 0) {
        strcat(flags->pattern + flags->lenPattern, "|");
        flags->lenPattern++;
    }
    flags->lenPattern += sprintf(flags->pattern + flags->lenPattern, "(%s)", pattern);
}

void addRegFromFile(flags_t *flags, char *filepath) {
    FILE *fp = fopen(filepath, "r");

    if (fp == NULL) {
        if (!flags->s) {
            perror(filepath);
            exit(1);
        }
    }
    char *line = NULL;
    size_t len = 0;
    int read = getline(&line, &len, fp);
    while (read != -1) {
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        patternAdd(flags, line);
        read = getline(&line, &len, fp);
    }
    free(line);
    fclose(fp);
}

void printMath(regex_t *re, char *line) {
    regmatch_t math;
    int offset = 0, flag = 1;
    while (flag) {
        int result = regexec(re, line + offset, 1, &math, 0);
        if (result != 0) {
            flag = 0;
        }
        for (int i = math.rm_so; i < math.rm_eo; ++i) {
            putchar(line[i]);
        }
        putchar('\n');
        offset += math.rm_eo;
    }
}

void printFile(flags_t flags, char *path, regex_t *reg) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        if (!flags.s) {
            perror(path);
            exit(1);
        }
    }

    char *line = NULL;
    size_t len = 0;
    int read = 0, lineCount = 1, countMatchingStrings = 0;

    read = getline(&line, &len, fp);
    while (read != -1) {
        int result = regexec(reg, line, 0, NULL, 0);

        if ((result == 0 && !flags.v) || (flags.v && result != 0)) {
            if (!flags.c && !flags.l) {
                if (!flags.h) {
                    printf("%s:", path);
                }
                if (flags.n) {
                    printf("%d:", lineCount);
                }
                if (flags.o) {
                    printMath(reg, line);
                } else {
                    outputLine(line, read);
                }
            }
            countMatchingStrings++;
        }
        read = getline(&line, &len, fp);
        lineCount++;
    }
    free(line);
    if (flags.c && !flags.l) {
        if (!flags.h) {
            printf("%s:", path);
        }
        printf("%d\n", countMatchingStrings);
    }
    if (flags.l && countMatchingStrings > 0) printf("%s\n", path);

    fclose(fp);
}
