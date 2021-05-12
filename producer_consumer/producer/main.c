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

#pragma region Click to expand preprocessor/macros section

#define DEBUG_MSGS          (1)
// As per specification, max size of the buffer is 1024 bytes
#define BUFFER_MAX_SIZE     (1024)
#define SEM_KEY             ((key_t)0xC0DE)
#define SHM_KEY             ((key_t)0xC0DE)
#define SEM_NAME            ("/sem_prodCons")
#define SHM_NAME            ("sharedMem_prodCons")

#pragma endregion

#pragma region Click to expand global variable declaration section

// Most commercial distributions of unix-like systems defines PATH_MAX in whatever convinient way in limits.h

typedef struct {
    int shmId;
    caddr_t pSharedMemory;
    sem_t *pSemaphore;
    struct sigaction sigAct;
    uint8_t filePath[PATH_MAX];
    size_t ul_fileSize;
    FILE *pFile;
    pid_t pidConsumer;
    bool bprodComplete;
} HousekeepingType_t;

#pragma endregion

#pragma region Click to expand Utility Code

size_t Utility_strncpy(uint8_t dest[], uint8_t src[], size_t limit)
{
    uint64_t index = 0;

    if((NULL == src) || (NULL == dest))
    {
        errno = EFAULT;
        return 0;
    }

    do {
        dest[index] = src[index];
        if('\0' == src[index++])
        {
            break;
        }
        
    } while(index != limit-1);

    dest[index] = '\0';
    return ((size_t) index);
}

pid_t Utility_getConsumerPID(void)
{
    char line[128];
    FILE *cmd = popen("pidof consumer.out", "r");
    fgets((char*)line, (int)128, cmd);
    pid_t pid = strtoul(line, NULL, 10);
    pclose(cmd);

    return pid;
}

bool Utility_doesFileExist (uint8_t *filename)
{
    struct stat temp;
    bool result = false;
    if(0 == stat ((char*)filename, &temp))
    {
        result = true;
    }
    return result;
}

void sigHandler_wokenUpbyConsumer(int sig)
{
    if(SIGUSR1 == sig)
    {
#ifdef DEBUG_MSGS    
        printf("\nWoken up by Consumer..\n");
#endif
    }
    else if(SIGSTOP == sig || SIGKILL == sig || 
            SIGQUIT == sig || SIGTERM == sig)
    {
#ifdef DEBUG_MSGS    
        printf("\nSomething went wrong..\n");
#endif
        kill(Utility_getConsumerPID(), SIGTERM);
    }   
}

void sig_setup(struct sigaction *psigAct, void (*handler)(int))
{
    psigAct->sa_handler = handler;
    // Initialize signal set as if it is empty
    sigemptyset(&psigAct->sa_mask);
    psigAct->sa_flags = 0;
    sigaction(SIGUSR1, psigAct, 0);
}

int main(int argc, char* argv[])
{
    HousekeepingType_t Producer;
 
    // Clean terminal
    system("clear");

    // Setup signals
#ifdef DEBUG_MSGS
    printf("\nSetting up signals");
#endif    
    sig_setup(&Producer.sigAct, sigHandler_wokenUpbyConsumer);

#ifdef DEBUG_MSGS
    printf("\nStarting Producer..");
#endif           

    if(0 == argc)
    {
        fprintf(stderr, "\nInsufficient information. Ensure following argument: path to the file to read.\n\n");
        exit(EXIT_FAILURE);
    }
    else if(argc >= 1)
    {
#ifdef DEBUG_MSGS
    printf("\nChecking if the file exists: %s", argv[1]);
#endif           
        if(false == Utility_doesFileExist((uint8_t*)argv[1]))
        {
            fprintf(stderr, "\nIncorrect input. File not present.\n\n");
            exit(EXIT_FAILURE);
        }

		// Copy the file path into user defined variable for convenience
#ifdef DEBUG_MSGS
		printf("\nSetting filepath..");
#endif           
        Utility_strncpy(Producer.filePath, (uint8_t*)argv[1], PATH_MAX);
    }
    else
    {
        // Every if-else if ladder shall have a else condition - One of the MISRA guidlines.
        ;
    }


#ifdef DEBUG_MSGS
    printf("\nOpening target file: %s", Producer.filePath);
#endif        
    // Open the file
    Producer.pFile = fopen((const char*)Producer.filePath, "r"); // read mode
    if (NULL == Producer.pFile)
    {
        printf("\n\terrno = %d\n", errno);
        fprintf(stderr, "\nError while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    // Go to EOF
    fseek(Producer.pFile, 0, SEEK_END);
    // Get length
    Producer.ul_fileSize = ftell(Producer.pFile);
    // Point to start of file again
    fseek(Producer.pFile, 0, SEEK_SET);
#ifdef DEBUG_MSGS
    printf("\nFile size is %lu bytes..", Producer.ul_fileSize);
#endif

    // Setup named semaphore
#ifdef DEBUG_MSGS
    printf("\nSetting up named semaphore");
#endif        
    Producer.pSemaphore = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    if(NULL == Producer.pSemaphore)
    {
        fprintf(stderr, "\nsem_open failed\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MSGS
    printf("\nSetting up shared memory segment");
#endif
        
    // Create a shared memory segment
    Producer.shmId = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if(-1 == Producer.shmId)
    {
        printf("\n\terrno = %d\n", errno);
        fprintf(stderr, "\nshm_open failed\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MSGS
    printf("\nSetting up shared memory segment size");
#endif        
    // Set the size
    if(-1 == ftruncate(Producer.shmId, Producer.ul_fileSize))
    {   
		#ifdef DEBUG_MSGS 
        printf("\n\terrno = %d\n", errno);
        #endif
        fprintf(stderr, "\ntruncating failed.\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_MSGS
    printf("\nMapping shared memory segment");
#endif        
    // Allocate 1024 bytes and map it 
    Producer.pSharedMemory = mmap(0, Producer.ul_fileSize, PROT_WRITE, MAP_SHARED, Producer.shmId, 0);
    if (NULL == Producer.pSharedMemory)
    {
		#ifdef DEBUG_MSGS
        printf("\n\terrno = %d\n", errno);
        #endif
        fprintf(stderr, "\nError with mmap.\n");
        exit(EXIT_FAILURE);
    }

    // Clean up before starting
    Producer.pidConsumer = 0;

#ifdef DEBUG_MSGS
    printf("\nGetting Consumer's pid");
#endif

    do {
        Producer.pidConsumer = Utility_getConsumerPID();
    } while (0 == Producer.pidConsumer);
#ifdef DEBUG_MSGS
    printf("\nConsumer's pid: %u", Producer.pidConsumer);
#endif

    while (1)
    {		
	    char ch = ' ';
		ch = fgetc(Producer.pFile);
		*Producer.pSharedMemory = ch;
		Producer.pSharedMemory++;
        if(ch == EOF)
		{
            break;
		}
    }


#ifdef DEBUG_MSGS
    printf("\nNothing more to read..");
#endif

	// Release Semaphore once reading is done
    sem_post(Producer.pSemaphore);			
    // Wait for signal from consumer thread
    pause();    
    // Relinquish semaphore
    sem_post(Producer.pSemaphore);
    // Destroy semaphore
    sem_destroy(Producer.pSemaphore);
    // Un-map the page
    munmap(Producer.pSharedMemory, Producer.ul_fileSize);
    // Close file
    close(Producer.shmId);
    // Unlink shared-memory object
    shm_unlink(SHM_NAME);
    // Close file
    fclose(Producer.pFile);
    // wrap-up
    printf("\n\nExit Success..\n\n");
    exit(EXIT_SUCCESS);
}
