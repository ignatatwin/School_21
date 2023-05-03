#include "s21_grep.h"

int main(int argc, char *argv[]) {
  grepInfo flags = {0};
  grepCounters counters = {0};

  parser(argc, argv, &flags);
  patterns(argv, &flags);
  readFile(argc, argv, &flags, &counters);

  return 0;
}

void parser(int argc, char *argv[], grepInfo *flags) {
  char option;
  while ((option = getopt_long(argc, argv, short_options, NULL, NULL)) != -1) {
    switch (option) {
      case 'e':
        flags->pattern = 1;
        patterns(argv, flags);
        break;
      case 'i':
        flags->ignore_case = 1;
        break;
      case 'v':
        flags->invert_match = 1;
        break;
      case 'c':
        flags->count_c = 1;
        break;
      case 'l':
        flags->files_match = 1;
        break;
      case 'n':
        flags->line_number = 1;
        break;
      case 'h':
        flags->without_filename = 1;
        break;
      case 's':
        flags->withot_error = 1;
        break;
      case 'f':
        flags->file_pattern = 1;
        patterns(argv, flags);
        break;
      case 'o':
        flags->count_match = 1;
        break;
      case '?':
      default:
        errorFlag();
    }
  }
}

void patterns(char *argv[], grepInfo *flags) {
  char buf[SIZE] = {0};
  if (!flags->pattern && !flags->file_pattern) {
    snprintf(pattern_only, sizeof(pattern_only), "%s", argv[optind]);
    optind++;
  }
  if (flags->pattern && !flags->file_pattern) {
    if (pattern_only[0] == 0) {
      snprintf(pattern_only, sizeof(pattern_only), "%s", optarg);
    } else {
      snprintf(buf, sizeof(pattern_only), "|%s", optarg);
      strcat(pattern_only, buf);
    }
  }
  if (flags->file_pattern) {
    filePattern(buf, flags);
  }
}

void readFile(int argc, char *argv[], grepInfo *flags, grepCounters *counters) {
  FILE *fp;
  int numeric = optind;
  char *name_file;

  while ((name_file = argv[numeric]) != NULL) {
    if (strcmp(name_file, "-") != 0) {
      if ((fp = fopen(argv[numeric], "r"))) {
        regexFile(fp, name_file, argc, counters, flags);
      } else {
        if (!flags->withot_error) {
          perror("NO FILE");
          error();
        }
      }
    }
    numeric++;
  }
}

void regexFile(FILE *fp, char *name_file, int argc, grepCounters *counters,
               grepInfo *flags) {
  int getLine = 0;
  int success = 0;
  counters->counter = 0;
  counters->success = 0;
  compilation(flags);

  pattern = (char *)calloc(SIZE, sizeof(strLenght));
  if (pattern == NULL) {
    error();
  }

  while ((getLine = getline(&pattern, &strLenght, fp)) != EOF) {
    printFile(argc, &success, name_file, flags, counters);
  }

  if (flags->count_c) {
    countCPlus(argc, flags, &success, name_file, counters);
  }

  if (flags->files_match && success > 0) {
    printf("%s\n", name_file);
  }

  if (flags->invert_match && flags->files_match && success == 0) {
    printf("%s\n", name_file);
  }

  free(pattern);
  regfree(&regex);
}

void printFile(int argc, int *success, char *name_file, grepInfo *flags,
               grepCounters *counters) {
  counters->counter++;
  if ((counters->success = regexec(&regex, pattern, matchN, matchP, 0)) == 0) {
    *success += 1;
  }
  if (pattern[strlen(pattern) - 1] == '\n') {
    pattern[strlen(pattern) - 1] = '\0';
  }
  if (!flags->count_c) {
    if (!counters->success && !flags->invert_match && !flags->files_match &&
        !flags->count_match) {
      withoutFilename(argc, name_file, flags);
      if (flags->line_number) {
        printf("%d:", counters->counter);
      }
      printf("%s\n", pattern);
    }
    if (counters->success && flags->invert_match && !flags->files_match) {
      withoutFilename(argc, name_file, flags);
      if (flags->line_number) {
        printf("%d:", counters->counter);
      }
      printf("%s\n", pattern);
    }
    if (!counters->success && !flags->invert_match && !flags->files_match &&
        flags->count_match) {
      withoutFilename(argc, name_file, flags);
      if (flags->line_number) {
        printf("%d:", counters->counter);
      }
      char *first_string = pattern;
      for (unsigned int l = 0; l < strlen(first_string); l++) {
        if (regexec(&regex, first_string, matchN, matchP, 0)) {
          break;
        }
        int end = 0;
        for (size_t size = 0; size <= matchN; size++) {
          if (matchP[size].rm_so == -1) {
            break;
          }
          if (size == 0) {
            end = matchP[size].rm_eo;
          }
          char buf[strlen(first_string) + 1];
          strcpy(buf, first_string);
          buf[matchP[size].rm_eo] = 0;
          printf("%s\n", (buf + matchP[size].rm_so));
        }
        first_string += end;
      }
    }
  }
}

void countCPlus(int argc, grepInfo *flags, int *success, char *name_file,
                grepCounters *counters) {
  if (!flags->invert_match && !flags->files_match) {
    withoutFilename(argc, name_file, flags);
    printf("%d\n", *success);
  }
  if (flags->invert_match && !flags->files_match) {
    withoutFilename(argc, name_file, flags);
    printf("%d\n", (counters->counter - *success));
  }
  if (flags->invert_match && flags->files_match) {
    withoutFilename(argc, name_file, flags);
    if ((counters->counter - *success) > 0) {
      printf("1\n");
    } else {
      printf("0\n");
    }
  }
  if (!flags->invert_match && flags->files_match) {
    withoutFilename(argc, name_file, flags);
    if (*success > 0) {
      printf("%d\n", 1);
    } else {
      printf("%d\n", 0);
    }
  }
}

void compilation(grepInfo *flags) {
  int status;
  if (flags->ignore_case) {
    if ((status = regcomp(&regex, pattern_only, REG_ICASE)) != 0) {
      printf("failed - %d", status);
      error();
    }
  } else {
    if ((status = regcomp(&regex, pattern_only, REG_EXTENDED)) != 0) {
      printf("failed - %d", status);
      error();
    }
  }
}

void filePattern(char *buf, grepInfo *flags) {
  if (optarg != NULL) {
    FILE *fb;
    if ((fb = fopen(optarg, "r"))) {
      char file_pattern[SIZE_2] = {0};
      while (fgets(file_pattern, 1024, fb) != NULL) {
        if (file_pattern[strlen(file_pattern) - 1] == '\n') {
          file_pattern[strlen(file_pattern) - 1] = '\0';
        }
        if (pattern_only[0] == 0) {
          snprintf(pattern_only, sizeof(pattern_only), "%s", file_pattern);
        } else {
          snprintf(buf, sizeof(pattern_only), "|%s", file_pattern);
          strcat(pattern_only, buf);
        }
      }
    } else {
      if (flags->withot_error == 0) {
        error();
      }
      fclose(fb);
    }
  }
}

void withoutFilename(int argc, char *name_file, grepInfo *flags) {
  if ((argc - optind) > 1 && !flags->without_filename) {
    printf("%s:", name_file);
  }
}

void errorFlag() {
  perror("use: grep [-eivclnhsfo] template [file ...]");
  exit(1);
}

void error() {
  perror("error");
  exit(1);
}