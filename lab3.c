#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_SIZE 5

static void add_history(char *history[], int *next, size_t *count,
                        const char *line) {
  if (history[*next] != NULL) {
    free(history[*next]);
    history[*next] = NULL;
  }

  history[*next] = strdup(line);
  if (history[*next] == NULL) {
    perror("strdup");
    exit(EXIT_FAILURE);
  }

  *next = (*next + 1) % HISTORY_SIZE;
  (*count)++;
}

static void print_history(char *history[], size_t count, int next) {
  size_t num = (count < HISTORY_SIZE) ? count : HISTORY_SIZE;
  int start = (count < HISTORY_SIZE) ? 0 : next;

  for (size_t i = 0; i < num; ++i) {
    int idx = (start + (int)i) % HISTORY_SIZE;
    printf("%s\n", history[idx]);
  }
}

static void free_history(char *history[]) {
  for (int i = 0; i < HISTORY_SIZE; ++i) {
    free(history[i]);
    history[i] = NULL;
  }
}

int main(void) {
  char *history[HISTORY_SIZE] = {NULL};
  int next = 0;
  size_t count = 0;

  char *line = NULL;
  size_t linecap = 0;

  while (1) {
    printf("Enter input: ");
    fflush(stdout);

    ssize_t nread = getline(&line, &linecap, stdin);
    if (nread == -1) {
      printf("\n(Ended)\n");
      break;
    }

    if (nread > 0 && line[nread - 1] == '\n')
      line[nread - 1] = '\0';

    add_history(history, &next, &count, line);

    if (strcmp(line, "print") == 0) {
      print_history(history, count, next);
    }
  }

  free(line);
  free_history(history);
  return 0;
}
