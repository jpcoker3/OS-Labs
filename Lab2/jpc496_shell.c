#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_COMMANDS 10
#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64

char *history[MAX_COMMANDS];
int num_commands = 0;

// print the most recent 10 commands to the terminal
void print_history() {
  int i;
  for (i = 0; i < num_commands; i++) {
    printf("%d: %s\n", i + 1, history[i]);
  }
}

// add command to the history array
void add_history(char *input) {
  // Allocate memory for the input string
  char *command = malloc(strlen(input) + 1);
  if (!command) {
    fprintf(stderr, "Could not allocate memory for command.\n");
    return;
  }
  strcpy(command, input);

  // Add the command to the history array
  if (num_commands < MAX_COMMANDS) {
    history[num_commands] = command;
    num_commands++;
  } else {
    free(history[0]);
    memmove(history, history + 1, sizeof(char *) * (MAX_COMMANDS - 1));
    history[MAX_COMMANDS - 1] = command;
  }
}

// write the command history to the audit.log
void write_history() {
  FILE *file = fopen("audit.log", "w");
  if (!file) {
    fprintf(stderr, "Could not open audit.log for writing.\n");
    return;
  }

  int i;
  for (i = 0; i < num_commands && i < MAX_COMMANDS; i++) {
    if (history[i]) {
      fprintf(file, "%s\n", history[i]);
    }
  }

  fclose(file);
}

void handle_signal(int sig) {
  if (sig == SIGUSR1) {
    write_history(); // write history to audit.log
    printf("Caught USR1 signal\n");
    exit(0); // exit the program
  }
}

int main(int argc, char **argv) {
  char input[MAX_INPUT_LENGTH];

  signal(SIGUSR1, handle_signal);

  while (1) {
    printf("myshell>");
    fflush(stdout);

    fgets(input, MAX_INPUT_LENGTH, stdin);
    input[strcspn(input, "\n")] = 0; // remove newline

    // Add command to history
    add_history(input);

    if (strcmp(input, "") == 0) { // empty input
      continue;
    }

    if (strcmp(input, "history") == 0) { // history command
      print_history();
      continue;
    }

    if (strcmp(input, "env") == 0) { // env command
      char **env = __environ;
      while (*env) {
        printf("%s\n", *env);
        env++;
      }
      continue;
    }

    if (strncmp(input, "cd ", 3) == 0) { // cd command
      char *path = input + 3;
      chdir(path);
      continue;
    }

    if (strstr(input, "&")) { // background command
      char *args[MAX_ARGS];
      char *arg;
      int i = 0;

      input[strcspn(input, "&")] = 0; // remove ampersand

      arg = strtok(input, " ");
      while (arg != NULL) {
        args[i++] = arg;
        arg = strtok(NULL, " ");
      }
      args[i] = NULL;

      pid_t pid = fork();
      if (pid == 0) {          // child process
        execvp(args[0], args); // execute command
        exit(0);
      }
    } else { // foreground command
      char *args[MAX_ARGS];
      char *arg;
      int i = 0;

      arg = strtok(input, " ");
      while (arg != NULL) {
        args[i++] = arg;
        arg = strtok(NULL, " ");
      }
      args[i] = NULL;

      pid_t pid = fork();
      if (pid == 0) {          // child process
        execvp(args[0], args); // execute command
        exit(0);
      } else {      // parent process
        wait(NULL); // wait for child to finish
      }
    }
  }

  return 0;
}
