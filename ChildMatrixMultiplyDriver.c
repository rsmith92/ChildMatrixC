#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void main(int argc, char* argv[])
{
    int n;
    sscanf(argv[2], "%d", &n);
    char *prog = argv[1];               // set prog as the second program name
    char *argarr[4];
    argarr[3] = NULL;                   // last item in argument array is NULL
    argarr[0] = prog;                   // set first item in argument array as program name
    //printf("prog = %s\n",argarr[0]);
    argarr[2] = argv[2];                // third item in argument array is number of total children
    char ichar[3];
    for(int i = 0; i < n; i++)          // create a child n times as specified at the command line
    {
        if(fork() == 0)                 // create a child for 
        {
            //printf("making child\n");
            //fflush(0);
            sprintf(ichar, "%d", i);
            argarr[1] = ichar;          // second item in argument array is the childs number 
            execv(prog, argarr);        // run the other program with argument array: [program name, child number, total children number] as arguments
            perror("Error: ");
            printf("if you get here execv is failing\n");
            return;
        }
    }
    return;
}
