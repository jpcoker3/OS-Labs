#include <stdio.h>
#include <unistd.h>

// Referenced:
// https://www.digitalocean.com/community/tutorials/execvp-function-c-plus-plus

int main(void) {

  // NULL terminated array of char* strings
  char *mycmd[] = {"ls", "-l", "/usr", NULL};

  // will execute ls -l /usr
  // mycmd[0] == "ls", mycmd = "ls -l /usr"
  execvp(mycmd[0], mycmd);

  // will return -1 if error.
  int status_code = execvp("ls", mycmd);

  // if there is an error, print error message and return.
  if (status_code == -1) {
    printf("Process did not terminate correctly\n");
    return 0;
  }

  return 0;
}