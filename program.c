#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "connection.h"

#define FALHA 1

const int S = 4184, P = 1000, B = 4;
const float R= 0.001;

struct boiler_info info;

pthread_t tempController, boilerInfoReader;

pthread_t fileWriter;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void updateBoilerInfo(void) {
    while (1) {
        pthread_mutex_lock(&mutex);
            getBoilerInfo(&info);
        pthread_mutex_unlock(&mutex);


        printf("Air Temperature: %f \n", info.airTemp);
        printf("Water Temperature: %f \n", info.waterTemp);
        printf("Incoming Water Temperature: %f \n", info.waterInTemp);
        printf("Outcoming Water Flow: %f \n", info.waterOutFlow);
        printf("Water Level: %f \n", info.waterLevel);

        sleep(5);
    }
}

void updateFile(void) {
    FILE *file;

    if (file == NULL) {
        file = fopen("/home/thiago/Documents/studies/Pos-automacao/Tecnicas_I_S_Automatizados/Trabalho I/trabalho_tisa/out.txt", "w");
    }

    for (int i=0; i<5; i++) {

        const double text = info.waterLevel;
        fprintf(file, "Some text: %f\n", text);

        sleep(5);
    }

    printf("ACABOUUUUU \\O/");

    fclose(file);
}

// void * controle_p1(void * arg) { 
//     double P, integ, No, Ni, N, waterLevel, erro, H, HRef;
//     double long Ki = 3000000;
//     double long Kp = 1000000;
//     double Href = 1.45; 

//    while(1){
//       pthread_mutex_lock(&mutex);
//         erro = Href - H; 
//         P = Kp * erro; 
//         integ = integ + (erro * Ki);
//         N = P + integ; 
//         Ni = N + No;
//         setBoilerControl(WATER_IN_FLOW, Ni);
//       pthread_mutex_unlock(&mutex);
//    }
// }

void * controle_p2(void * arg) {
    double P, integ = 0, Ni = 1, H, erro, Tref = 30;
    double Qi, Q, Qe, Qt; 
    const double Ki = 30000;
    const double Kp = 10000;
    double Ta, T, Ti;

    while(1){
    pthread_mutex_lock(&mutex);
        H = info.waterLevel;
        Ta = info.airTemp;
        T = info.waterTemp;
        Ti = info.waterInTemp;

        erro = Tref - T;
        P = Kp * erro; 
        integ = integ + (erro * Ki);
        Qt = P + integ;
        Qi = Ni * S * (Ti - T);
        Qe = (T - Ta) / R;
        Q = Qt + Qe - Qi;
        printf("%f\n", Q);
        setBoilerControl(HEAT_FLOW, Q);
    pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
	char *host = "localhost";
	int port = 5858;
	openConnection(host, port);

    // updateBoilerInfo();

    char *msg = setBoilerControl(WATER_IN_FLOW, 10.0);
    printf("Control Response: %s \n", msg);

    pthread_create(&boilerInfoReader, NULL, (void *) updateBoilerInfo, NULL);
    pthread_create(&tempController, NULL, (void *) controle_p2, NULL);
    
    //pthread_create(&fileWriter, NULL, (void *) updateFile, NULL);

    pthread_join(boilerInfoReader, NULL);
    pthread_join(tempController, NULL);
    //pthread_join(fileWriter, NULL);
}