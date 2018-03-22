#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "monitor.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t highTemperature;

double tempThreshould, currentTemp = 0;

struct boiler_info sharedInfo;

void writeBoilerInfo(struct boiler_info *newInfoValues)
{ 
    pthread_mutex_lock(&mutex);
        sharedInfo.airTemp = newInfoValues->airTemp;
        sharedInfo.waterTemp = newInfoValues->waterTemp;
        sharedInfo.waterInTemp = newInfoValues->waterInTemp;
        sharedInfo.waterOutFlow = newInfoValues->waterOutFlow;
        sharedInfo.waterLevel = newInfoValues->waterLevel;

        if (sharedInfo.waterTemp >= tempThreshould) {
            pthread_cond_signal(&highTemperature);
        }
    pthread_mutex_unlock(&mutex);
}

void readBoilerInfo(struct boiler_info *info)
{
    pthread_mutex_lock(&mutex);
        info->airTemp = sharedInfo.airTemp;
        info->waterTemp = sharedInfo.waterTemp;
        info->waterInTemp = sharedInfo.waterInTemp;
        info->waterOutFlow = sharedInfo.waterOutFlow;
        info->waterLevel = sharedInfo.waterLevel;
    pthread_mutex_unlock(&mutex);
}

void monitorTemperature(double tempThreshouldV) {
    tempThreshould = tempThreshouldV;

    pthread_mutex_lock(&mutex);
        while (currentTemp < tempThreshould) {
            pthread_cond_wait(&highTemperature, &mutex);
        }
    pthread_mutex_unlock(&mutex);
}
