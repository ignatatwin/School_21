#ifndef SRC_LIB_H_
#define SRC_LIB_H_

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int number_empty;
  int number_all;
  int show_endl;
  int squeeze;
  int show_tabs;
  int show_nonprinting;
  int error;
} catInfo;

void noArgc(FILE *fp, int argc, char *argv[]);
void catArgc(int argc, char *argv[]);
void parseArgs(catInfo *info, int argc, char *argv[]);
void readFile(char *argv[], int argc, catInfo *info);
void flagB(char ch, int *count_b, int *start_line);
void flagN(char ch, int *start_line, int *count_n, int count_line);
void flagE(char ch, int *stop_print, int empty_line, catInfo *info);
void flagS(char ch, int *count_line);
void flagT(char ch, int *stop_print);
void flagV(char ch, int *stop_print);

#endif  //  SRC_LIB_H_