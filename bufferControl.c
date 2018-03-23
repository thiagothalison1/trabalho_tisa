#include "bufferControl.h"
#include <pthread.h>

#define TAMBUF 100

struct buffer_data buffer[2][TAMBUF];

int bufferFree = 0;
int bufferFreeIndex = 0;
int bufferFull = -1;

pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bufferFullAlarm = PTHREAD_COND_INITIALIZER;

void insertValue (double waterLevel, double waterTemp) {
    pthread_mutex_lock(&bufferMutex);
        buffer[bufferFree][bufferFreeIndex].waterLevel = waterLevel;
        buffer[bufferFree][bufferFreeIndex].waterTemp = waterTemp;
        bufferFreeIndex++;

        if (bufferFreeIndex == TAMBUF) {
            bufferFull = bufferFree;
            bufferFree = (bufferFree + 1) % 2;
            bufferFreeIndex = 0;
            pthread_cond_signal(&bufferFullAlarm);
        }
    pthread_mutex_unlock(&bufferMutex);
}

struct buffer_data * waitFullBuffer (void) {
    struct buffer_data * bufferFullPointer;

    pthread_mutex_lock(&bufferMutex);
        while (bufferFull == -1){
            pthread_cond_wait(&bufferFullAlarm, &bufferMutex);
        }
        bufferFullPointer = (struct buffer_data *) &buffer[bufferFull];
        bufferFull = -1;
    pthread_mutex_unlock(&bufferMutex);

    return bufferFullPointer;
}


