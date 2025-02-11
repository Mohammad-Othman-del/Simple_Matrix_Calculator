#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <string.h>

#define MAX_INPUT 128

typedef struct {
    pthread_mutex_t mutex;
    char input_1[MAX_INPUT];
    char input_2[MAX_INPUT];
    char input_3[MAX_INPUT];
} SharedMemory;

void write_to_shared_memory() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof (SharedMemory), 0666 | IPC_CREAT);
    SharedMemory *shm = (SharedMemory*) shmat(shmid, NULL, 0);

    pthread_mutex_unlock(&shm->mutex);

    fgets(shm->input_1, MAX_INPUT, stdin);
    shm->input_1[strcspn(shm->input_1, "\n")] = 0;

    if (strcmp(shm->input_1, "END") == 0 ) {
        exit(0);
    }
    fgets(shm->input_2, MAX_INPUT, stdin);
    shm->input_2[strcspn(shm->input_2, "\n")] = 0;

    if (strcmp(shm->input_2, "END") == 0 ) {
        exit(0);
    }

    fgets(shm->input_3, MAX_INPUT, stdin);
    shm->input_3[strcspn(shm->input_3, "\n")] = 0;

    if (strcmp(shm->input_3, "END") == 0 ) {
        exit(0);
    }

    pthread_mutex_lock(&shm->mutex);

    shmdt(shm);
}

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(SharedMemory), 0666 | IPC_CREAT);
    SharedMemory *shm = (SharedMemory*) shmat(shmid, NULL, 0);

    if (shm->mutex.__data.__lock == 0) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&shm->mutex, &attr);
    }

    shmdt(shm);
    while (1) {
        write_to_shared_memory();
    }
}