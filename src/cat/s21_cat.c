//  Copyright [2022] <nashitet>
#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    noArgc(stdin, argc, argv);
  } else {
    catArgc(argc, argv);
  }
  return 0;
}

void noArgc(FILE *fp, int argc, char *argv[]) {
  char ch;
  if (argc == 2) {
    if (!(fp = fopen(argv[argc - 1], "r"))) {
      perror(argv[argc - 1]);
    } else {
      while ((ch = fgetc(fp)) != EOF) {
        printf("%c", ch);
      }
    }
    fclose(fp);
  } else if (argc == 1) {
    while (scanf("%c", &ch) != 10) {
      printf("%c", ch);
    }
  }
}

void catArgc(int argc, char *argv[]) {
  catInfo info = {0, 0, 0, 0, 0, 0, 1};
  int count = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] != '-') {
      parseArgs(&info, argc, argv);
      count++;
    }
  }
  if (info.error == 1 && count > 0) {
    readFile(argv, argc, &info);
  } else if (count == 0) {
    noArgc(NULL, argc, argv);
  }
}

void parseArgs(catInfo *info, int argc, char *argv[]) {
  int option = 0;
  int option_index = 0;
  static struct option long_options[] = {{"number-nonblank", 0, 0, 'b'},
                                         {"show-ends", 0, 0, 'E'},
                                         {"number", 0, 0, 'n'},
                                         {"show-ends", 0, 0, 'e'},
                                         {"squeeze-blank", 0, 0, 's'},
                                         {"show-nonprinting", 0, 0, 'v'},
                                         {"show-tabs", 0, 0, 't'},
                                         {"show-tabs", 0, 0, 'T'},
                                         {0, 0, 0, 0}};

  while ((option = getopt_long(argc, argv, "+benstvET", long_options,
                               &option_index)) != -1) {
    switch (option) {
      case 's':
        info->squeeze = 1;
        break;
      case 'n':
        info->number_all = 1;
        break;
      case 'b':
        info->number_empty = 1;
        info->number_all = 0;
        break;
      case 'e':
      case 'E':
        info->show_endl = 1;
        info->show_nonprinting = 1;
        break;
      case 't':
      case 'T':
        info->show_tabs = 1;
        info->show_nonprinting = 1;
        break;
      case 'v':
        info->show_nonprinting = 1;
        break;
      default:
        info->error = 1;
    }
  }
}

void readFile(char *argv[], int argc, catInfo *info) {
  FILE *fp = NULL;
  char ch;

  for (int i = 0; i < argc - 1; i++) {
    if (argv[i + 1][0] != '-') {
      fp = fopen(argv[i + 1], "r");
      int count_n = 1;
      int count_b = 1;
      int start_line = 0;
      int count_line = 0;
      int stop_print = 1;

      int empty_line = 0;
      int sumb = 0;

      if (!fp) {
        perror(argv[i + 1]);
      } else {
        while ((ch = getc(fp)) != EOF) {
          stop_print = 1;
          sumb++;
          empty_line = 0;
          if (ch == '\n' && sumb == 1) {
            empty_line = 1;
            sumb = 0;
          }
          if (ch == '\n' && sumb > 0) {
            sumb = 0;
          }
          if (info->squeeze == 1) {
            flagS(ch, &count_line);
          }
          if (info->number_empty == 1) {
            flagB(ch, &count_b, &start_line);
            info->number_all = 0;
          }
          if (info->number_all == 1) {
            flagN(ch, &start_line, &count_n, count_line);
          }
          if (info->show_endl == 1 && count_line != 2) {
            flagE(ch, &stop_print, empty_line, info);
          }
          if (info->show_tabs == 1) {
            flagT(ch, &stop_print);
          }
          if (info->show_nonprinting == 1) {
            flagV(ch, &stop_print);
          }
          if (stop_print == 1 && count_line != 2) {
            printf("%c", ch);
          }
        }
      }
    }
  }
}

void flagN(char ch, int *start_line, int *count_n, int count_line) {
  if (*start_line == 0 && count_line != 2) {
    printf("%6d\t", *count_n);
    *count_n += 1;
    *start_line = 1;
  }
  if (ch == '\n') {
    *start_line = 0;
  }
}

void flagB(char ch, int *count_b, int *start_line) {
  if (*start_line == 0 && ch != '\n') {
    printf("%6d\t", *count_b);
    *count_b += 1;
    *start_line = 1;
  } else if (ch == '\n') {
    *start_line = 0;
  }
}

void flagE(char ch, int *stop_print, int empty_line, catInfo *info) {
  if (ch == '\n') {
    if (info->number_empty != 1) {
      printf("$%c", ch);
      *stop_print = 0;
    }
    if (info->number_empty == 1 && empty_line == 1) {
      printf("      \t$%c", ch);
      *stop_print = 0;
    }
    if (info->number_empty == 1 && empty_line != 1) {
      printf("$%c", ch);
      *stop_print = 0;
    }
  }
}

void flagS(char ch, int *count_line) {
  if (ch != 10) {
    *count_line = -1;
  } else if (*count_line != 2) {
    *count_line += 1;
  }
}

void flagT(char ch, int *stop_print) {
  if (ch == 9) {
    printf("^I");
    *stop_print = 0;
  }
}

void flagV(char ch, int *stop_print) {
  if (ch >= 0 && ch < 32 && ch != 9 && ch != 10) {
    printf("^%c", ch + 64);
    *stop_print = 0;
  } else if (ch == 127) {
    printf("^?");
    *stop_print = 0;
  }
}
