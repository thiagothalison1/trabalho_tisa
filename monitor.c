#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "monitor.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t tempAlarm = PTHREAD_COND_INITIALIZER;

double tempLowerBound, tempUpperBound;

struct boiler_info sharedInfo;

void checkTemperatureAlarm()
{
    if (sharedInfo.waterTemp <= tempLowerBound || sharedInfo.waterTemp >= tempUpperBound) {
        pthread_cond_signal(&tempAlarm);
    }
}

void writeBoilerInfo(struct boiler_info *newInfoValues)
{
    pthread_mutex_lock(&mutex);
        sharedInfo.airTemp = newInfoValues->airTemp;
        sharedInfo.waterTemp = newInfoValues->waterTemp;
        sharedInfo.waterInTemp = newInfoValues->waterInTemp;
        sharedInfo.waterOutFlow = newInfoValues->waterOutFlow;
        sharedInfo.waterLevel = newInfoValues->waterLevel;
        checkTemperatureAlarm();
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

void monitorTemperature(double tempLowerBoundV, double tempUpperBoundV, double *temperatureInfo) {
    tempLowerBound = tempLowerBoundV;
    tempUpperBound = tempUpperBoundV;

    pthread_mutex_lock(&mutex);
        while (sharedInfo.waterTemp > tempLowerBound && sharedInfo.waterTemp < tempUpperBound) {
            pthread_cond_wait(&tempAlarm, &mutex);
        }
        double currentTemperature = sharedInfo.waterTemp;
        temperatureInfo = &currentTemperature;
    pthread_mutex_unlock(&mutex);
}
