#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<stdint.h>
#include<errno.h>
#include<fcntl.h>
#include<limits.h>
#include<linux/limits.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/mman.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/wait.h>

#define DEBUG_MSGS          (1)

#pragma region Click to expand preprocessor macros section
// As per specification, max size of the buffer is 1024 bytes
#define BUFFER_MAX_SIZE     (1024)
#define SEM_NAME            ("/sem_prodCons")
#define SHM_NAME            ("/sharedMem_prodCons")

#pragma endregion

#pragma region Click to expand global variable section

typedef struct {
    int shmId;
    caddr_t pSharedMemory;
    sem_t *pSemaphore;
    struct sigaction sigAct;
    pid_t pidProducer;
} HousekeepingType_t;

#pragma endregion

pid_t Utility_getProducerPID(void)
{
    char line[128];
    FILE *cmd = popen("pidof producer.out", "r");
    fgets((char*)line, (int)128, cmd);
    pid_t pid = strtoul(line, NULL, 10);
    pclose(cmd);

    return pid;
}

int main(int argc, char* argv[]) 
{ 
    HousekeepingType_t Consumer;
    unsigned long ul_reqSize = 0;
    
    // Clean terminal
    system("clear");

    // Open named semaphore
    Consumer.pSemaphore = sem_open(SEM_NAME, 0);
    if(NULL == Consumer.pSemaphore)
    {
        printf("\n\terrno = %d\n", errno);
        fprintf(stderr, "\nsem_open failed\n");
        exit(EXIT_FAILURE);
    }

    // Create and map to the shared memory segment
    Consumer.shmId = shm_open(SHM_NAME, O_RDWR, S_IRWXU | S_IRWXG);
    if(-1 == Consumer.shmId)
    {
        printf("\n\terrno = %d\n", errno);
        fprintf(stderr, "\nshm_open failed.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate required number of buffers and map it 
    Consumer.pSharedMemory = mmap(0, 2048, PROT_READ, MAP_SHARED, Consumer.shmId, 0);
    if (NULL == Consumer.pSharedMemory)
    {
        printf("\n\terrno = %d\n", errno);
        fprintf(stderr, "\nError with mmap.\n");
        exit(EXIT_FAILURE);
    }

    // Clean up before starting
    Consumer.pidProducer = 0;

#ifdef DEBUG_MSGS
    printf("\nGetting Producer's pid");
#endif        
    do {
        Consumer.pidProducer = Utility_getProducerPID();
        printf("\nProducer's pid: %u", Consumer.pidProducer);
    } while (0 == Consumer.pidProducer);

    sleep(3);

    printf("\nContent is:\n\n");

    sem_wait(Consumer.pSemaphore);

    while(1)
    {
        printf("%c", *Consumer.pSharedMemory);
        Consumer.pSharedMemory++;
        if(*Consumer.pSharedMemory == EOF)
        {
            break;
        }
    }

    kill(Consumer.pidProducer, SIGUSR1);
    // Un-map the page
    munmap(Consumer.pSharedMemory, ul_reqSize);
    // Close file
    close(Consumer.shmId);
    // Unlink shared-memory object
    shm_unlink(SHM_NAME);
    // wrap-up
    printf("\n\nExit Success..\n\n");
    exit(EXIT_SUCCESS); 
} 
