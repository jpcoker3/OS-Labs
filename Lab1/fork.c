#include <stdio.h>
#include <unistd.h>

// Referenced: https://www.geeksforgeeks.org/fork-system-call/

int main(void) {
  // counter to keep track of loop.
  // used for both parent and child
  int counter = 1;

  // if fork is successful
  if (fork() == 0) {
    while (counter <= 20) {
      printf("Child: %i \n", counter);
      counter += 1;
    }

    // If fork error
  } else if (fork() <= -1) {
    printf("COULD NOT FORK \n");
  }

  // else parent process
  else {
    while (counter <= 20) {
      printf("Parent: %i \n", counter);
      counter += 1;
    }
  }

  return 0;
}