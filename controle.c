#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "bufferControl.h"
#include "monitor.h"

#define FALHA 1
#define NSEC_PER_SEC (1000000000) /* The number of nsecs per sec. */

const int S = 4184, PAGUA = 1000, B = 4;
const float R= 0.001;
float Ni = 0;

pthread_t tempController, boilerInfoReader, levelController, temperatureAlarm, userInterface, logGenerator;

pthread_t fileWriter;

void alarmClock(int milisecInterval, struct timespec *t) {
    t->tv_nsec += milisecInterval * 1000000;

    while (t->tv_nsec >= NSEC_PER_SEC) {
        t->tv_nsec -= NSEC_PER_SEC;
        t->tv_sec++;
    }

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, t, NULL);
}

void updateBoilerInfo(void) {
    struct boiler_info info;
    int infoReaderPeriod = 115;
    struct timespec infoReaderClock;
    clock_gettime(CLOCK_MONOTONIC ,&infoReaderClock);


    while (1) {
        getBoilerInfo(&info);
        writeBoilerInfo(&info);
        insertValue(info.waterLevel, info.waterTemp);
        alarmClock(infoReaderPeriod, &infoReaderClock);
    }
}

void updateFile(void) {
    // FILE *file;

    // if (file == NULL) {
    //     file = fopen("/home/thiago/Documents/studies/Pos-automacao/Tecnicas_I_S_Automatizados/Trabalho I/trabalho_tisa/out.txt", "w");
    // }

    // for (int i=0; i<5; i++) {

    //     const double text = info.waterLevel;
    //     fprintf(file, "Some text: %f\n", text);

    //     sleep(5);
    // }

    // printf("ACABOUUUUU \\O/");

    // fclose(file);
}

void * levelControl(void * arg) { 
    struct boiler_info info;
    double prop, I=0, No, N, erro, H, IOld;
    double long Ki = 1159;
    double long Kp = 5883;
    double Href = 2.1;

    int controllerPeriod = 150;
    struct timespec levelControllerClock;
    clock_gettime(CLOCK_MONOTONIC ,&levelControllerClock);

   while(1){
    readBoilerInfo(&info);

    H = info.waterLevel;
    No = info.waterOutFlow;

    erro = Href - H; 
    prop = Kp * erro; 
    I = IOld + (erro * Ki);
    N = prop + I; 
    double NiTemp = N + No;
    Ni = (NiTemp > 100) ? 100 : (NiTemp < 0) ? 0 : NiTemp;

    setBoilerControl(WATER_IN_FLOW, Ni);
    alarmClock(controllerPeriod, &levelControllerClock);
   }
}

void * temperatureControl(void * arg) {
    struct boiler_info info;
    double prop, I = 0, erro, Tref = 21, IOld;
    double Qi, Q, Qe, Qt; 
    const double Ki = 50000;
    const double Kp = 87500;
    double Ta, T, Ti;

    int controllerPeriod = 200;
    struct timespec temperatureControllerClock;
    clock_gettime(CLOCK_MONOTONIC ,&temperatureControllerClock);

    while(1){
        readBoilerInfo(&info);

        Ta = info.airTemp;
        T = info.waterTemp;
        Ti = info.waterInTemp;

        erro = Tref - T;
        prop = Kp * erro;
        I = IOld + (erro * Ki);
        Qt = prop + I;
        Qi = Ni * S * (Ti - T);
        Qe = (T - Ta) / R;
        Q = Qt + Qe - Qi;
        Q = (Q > 1000000) ? 1000000 : (Q < 0) ? 0 : Q;
       
        setBoilerControl(HEAT_FLOW, Q);
        alarmClock(controllerPeriod, &temperatureControllerClock);
    }
}

void * watchTemperature(void) {
    double temperatureInfo;
    double tempLowerBound = 16;
    double tempUpperBound = 25;

    int temperatureAlarmPeriod = 1000;
    struct timespec temperatureAlarmClock;
    clock_gettime(CLOCK_MONOTONIC ,&temperatureAlarmClock);

    while(1) {
        monitorTemperature(tempLowerBound, tempUpperBound, &temperatureInfo);
        printf("Critical Temperature: %f\n", temperatureInfo);
        alarmClock(temperatureAlarmPeriod, &temperatureAlarmClock);
    }

}

void * informUser(void) {
    int period = 1000;
    struct timespec myClock;
    clock_gettime(CLOCK_MONOTONIC ,&myClock);
    struct boiler_info info;
    
    while (1) {
        readBoilerInfo(&info);
        printf("Water level: %f\nTemperature: %f\n\n", info.waterLevel, info.waterTemp);
        alarmClock(period, &myClock);
    }
}

void * logInfo() {
    FILE *file;

    while (1) {
        struct buffer_data * fullBuffer = waitFullBuffer();

        file = fopen("./out.txt", "a");

        struct buffer_data * data;

        for (int i = 0; i < 100; i++) {
            data = &fullBuffer[i];
            fprintf(file, "Water Temp: %f\n", data->waterTemp);
            fprintf(file, "Water Level: %f\n", data->waterLevel);
        }

        fclose(file);
    }
}

int main(int argc, char *argv[])
{
	char *host = "localhost";
	int port = 5858;
	openConnection(host, port);

    pthread_create(&boilerInfoReader, NULL, (void *) updateBoilerInfo, NULL);
    pthread_create(&tempController, NULL, (void *) temperatureControl, NULL);
    pthread_create(&levelController, NULL, (void *) levelControl, NULL);
    pthread_create(&userInterface, NULL, (void *) informUser, NULL);
    pthread_create(&temperatureAlarm, NULL, (void *) watchTemperature, NULL);
    pthread_create(&logGenerator, NULL, (void *) logInfo, NULL);
    
    pthread_join(boilerInfoReader, NULL);
    pthread_join(tempController, NULL);
    pthread_join(levelController, NULL);
    pthread_join(userInterface, NULL);
    pthread_join(temperatureAlarm, NULL);
    pthread_join(logGenerator, NULL);
}