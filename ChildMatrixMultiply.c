#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// syncronize the child processes before they perform an operation
void synch(int par_id,int par_count,int *ready)
{
    int synchid = ready[par_count]+1;
    ready[par_id]=synchid;
    int breakout = 0;
    while(1)
    {
        breakout=1;
        for(int i=0;i<par_count;i++)
        {
            if(ready[i]<synchid)
            {
                breakout = 0;break;
            }
        }
        if(breakout==1)
        {
            ready[par_count] = synchid; break;
        }
    }
}

// take in float array and randomize the values in it 
void randomizer(float* arr)
{
    float random;
    time_t t;
   
    srand((unsigned) time(&t));

    int arrsize = 1024;
    for(int k = 0; k < 1024; k++)
    {
        random = (float) rand() / (float) RAND_MAX;
        random*=10.0;
        arr[k] = random;
    }
    return;
}

// function executes arrA * arrB = arrC and returns void. arrA and arrB are unchanged
void matrixmultiply(float *arrC, float* arrA, float* arrB, int pc, int id) //A * B = C
{
    int width = 32;
    int val = 32/pc;
    float answer, a, b;
    for(int j = id * val; j < (id + 1) * val; j++)
    {
        //printf("j = %d & id = %d\n", j, id);
        //fflush(0);
        for(int initial = 0; initial < width; initial++)
        {
            answer = 0;
            for(int i = 0; i < width; i++)
            {
                a = arrA[(j*width) + i];
                b = arrB[(width*i) + initial]; // double check this
                answer = answer + (a * b);
            }
            //ADD NEW ENTRY TO RESULT
            //printf("answer = %f\n", answer);
            //fflush(0);
            arrC[(j*width) + initial] = answer;
        }
    }
}

// function will print the matrix values in a somewhat readable format
void print_matrix(float* m)//for 32*32
{
    printf("_______________________________________________________\n");
    for (int r = 0; r < 32; r++)
        {
        for (int c = 0; c < 32; c++)
            printf("%.1f  ", m[c + r * 32]);            
        printf("\n");
        }
    printf("_______________________________________________________\n");
}

void main(int argc, char *argv[])
{
    // should be passed in with execv - 0 to 15
    int kidnum = atoi(argv[1]); 
    int n = atoi(argv[2]);
    int width = 32;
    int arrsize = 1024;
    //initialize name shared ready int*
        int fd =shm_open("ready", O_RDWR|O_CREAT,0777); 
        ftruncate(fd, 16*sizeof(int)); 
    int *ready = (int*)mmap(NULL, 16*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    // initialize name shared memory arrA, arrB, arrC of size arrsize
        int fd1 =shm_open("arrA", O_RDWR|O_CREAT,0777);
        ftruncate(fd1, arrsize*sizeof(float));
    float *arrA = (float*)mmap(NULL, arrsize*sizeof(float), PROT_READ|PROT_WRITE, MAP_SHARED, fd1, 0);

        int fd2 =shm_open("arrB", O_RDWR|O_CREAT,0777);
        ftruncate(fd2, arrsize*sizeof(float));
    float *arrB = (float*)mmap(NULL, arrsize*sizeof(float), PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0);

        int fd3 =shm_open("arrC", O_RDWR|O_CREAT,0777);
        ftruncate(fd3, arrsize*sizeof(float));
    float *arrC = (float*)mmap(NULL, arrsize*sizeof(float), PROT_READ|PROT_WRITE, MAP_SHARED, fd3, 0);

    // first child process randomizes the values
    if(kidnum == 0)
    {
        printf("randomizing...\n");
        fflush(0);
        randomizer(arrA);
        randomizer(arrB);
        randomizer(arrC);
        for(int numb = 0; numb < 16; numb++)
        {
            ready[numb] = 0;
        }
    }
    // rest of the children wait for a moment
    else
    {
        sleep(1);
    }
    

    // synchronize children
    synch(kidnum, n, ready);

    // first time measurement 
    clock_t a = clock();

    // call multiplication first time
    matrixmultiply(arrC, arrA, arrB, n, kidnum);

    // synchronize children
    synch(kidnum, n, ready);

    // call multiplication second time
    matrixmultiply(arrA, arrB, arrC, n, kidnum);

    // synchronize children
    synch(kidnum, n, ready);

    // call multiplication third time
    matrixmultiply(arrB, arrA, arrC, n, kidnum);

    // synchronize children before printing time
    synch(kidnum, n, ready);

    // second time measurement
    clock_t b = clock(); 
    float cps = CLOCKS_PER_SEC;
    float time = ((float)b - (float)a) / cps;
    // child 0 will be the one to print the output
    if(kidnum == 0)
    {
        printf("The process took %f seconds.\n", time);
        fflush(0);

    // function prints a matrix (in this case arrC)
        print_matrix(arrC);

    // cleanup for all name shared memory
        close(fd);
        close(fd1);
        close(fd2);
        close(fd3);

        shm_unlink("ready");
        shm_unlink("arrA");
        shm_unlink("arrB");
        shm_unlink("arrC");

        munmap(ready, n*sizeof(int));
        munmap(arrA, arrsize*sizeof(float));
        munmap(arrB, arrsize*sizeof(float));
        munmap(arrC, arrsize*sizeof(float));
        printf("closing\n");
        fflush(0);
    }
    return;
}
