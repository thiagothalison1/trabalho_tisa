#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "monitor.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t temperatureAlarm;

double tempLowerBound, tempUpperBound;

struct boiler_info sharedInfo;

// void evaluateTemperature()
// {
//     while (sharedInfo.waterTemp >= tempUpperBound || sharedInfo.waterTemp <= tempLowerBound) {
//         pthread_cond_signal(&temperatureAlarm);
//     }
// }

void writeBoilerInfo(struct boiler_info *newInfoValues)
{
    pthread_mutex_lock(&mutex);
        sharedInfo.airTemp = newInfoValues->airTemp;
        sharedInfo.waterTemp = newInfoValues->waterTemp;
        sharedInfo.waterInTemp = newInfoValues->waterInTemp;
        sharedInfo.waterOutFlow = newInfoValues->waterOutFlow;
        sharedInfo.waterLevel = newInfoValues->waterLevel;
        // evaluateTemperature();
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

void monitorTemperature(double tempLowerBoundV, double tempUpperBoundV) {
    printf("oi");
    // //tempLowerBound = tempLowerBoundV;
    // //tempUpperBound = tempUpperBoundV;

    // pthread_mutex_lock(&mutex);
    //     //if (hasUpdates == -1 || sharedInfo.waterTemp > tempLowerBound && sharedInfo.waterTemp < tempUpperBound) {
    //     while (alarme == -1) {
    //         printf("entrei no wait\n");
    //         pthread_cond_wait(&temperatureAlarm, &mutex);
    //     }
    //     //}
    // pthread_mutex_unlock(&mutex);
    // printf("Water temp: %f\n", sharedInfo.waterTemp);
}
