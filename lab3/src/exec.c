#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main( int argc, char **argv ) 
{
    int pid = fork();
    
	if (pid == 0) {
	    execl("sequential_min_max", "./sequential_min_max", "123456789", "10000", NULL);
	    return 0;
    }

    wait(NULL);
    
	return 0;
}