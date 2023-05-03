#ifndef S21_GREP_H_
#define S21_GREP_H_

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 2056
#define SIZE_2 2055
#define MP 2
#define SIZE_STR 200

const char *short_options = "e:ivclnhsf:o?+";

typedef struct flags {
  int pattern;
  int ignore_case;
  int invert_match;
  int count_c;
  int files_match;
  int line_number;

  int without_filename;
  int withot_error;
  int file_pattern;
  int count_match;

} grepInfo;

typedef struct counters {
  int counter;
  int success;

} grepCounters;

regex_t regex;
regmatch_t matchP[MP];
size_t matchN = MP;
size_t strLenght = SIZE_STR;
char pattern_only[SIZE];
char *pattern = {0};

void parser(int argc, char *argv[], grepInfo *flags);
void patterns(char *argv[], grepInfo *flags);
void readFile (int argc, char *argv[], grepInfo *flags, grepCounters *counters);
void printFile (int argc, int *success, char *name_file, grepInfo *flags, grepCounters *counters);

void regexFile (FILE *fp, char *name_file, int argc, grepCounters *counters, grepInfo *flags);
void countCPlus (int argc, grepInfo *flags, int *success, char *name_file, grepCounters *counters);
void compilation (grepInfo *flags);
void filePattern (char *buf, grepInfo *flags);
void withoutFilename (int argc, char *name_file, grepInfo *flags);

void errorFlag();
void error();

#endif  // S21_GREP_H_
