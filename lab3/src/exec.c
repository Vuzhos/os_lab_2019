#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]) {
  execvp("./sequential_min_max", argv);
  printf("Error: execvp() failed, errno=%d\n", errno);
  return 0;
}