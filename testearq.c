#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define NUMTHREADS 5
#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */

unsigned int sleep(unsigned int seconds);

pthread_t threads[2];

pthread_mutex_t em = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
   int idx, lenght;
    const int S = 4184, P = 1000, B = 4;
    const float R= 0.001;
  /* double P, integ, No, Ni, N, H, erro, T, TRef;
   double Ki = 3000000;
   double Kp = 1000000;
   double Href = 1.45; */
}thread_arg, *ptr_thread_arg; //struct sensores_t valor; 

   void * controle_p1(void * arg){ // controle de temperatura   
	   double P, integ, No, Ni, N, H, erro, H, HRef;
	   double long Ki = 3000000;
	   double long Kp = 1000000;
	   double Href = 1.45; 

   while(1){

      pthread_mutex_lock(&em);
         erro = Href - H; 
         P = Kp * erro; 
         integ = integ + (erro * Ki);
         N = P + integ; 
         Ni = N + No;
       pthread_mutex_unlock(&em);
         //resto
   }
//---------------------------------------------------------------------------------------
   }

   void * controle_p2(void * arg){ //controle de nivel 
   	   double P, integ, Ni, H, erro, T, TRef;
   	   double Qi, Q, Qe, Qt; 
	   double long Ki = 3000000000;
	   double long Kp = 1000000000;
	   double TRef = 30; 
	   float Ta, T, Ti;

      while(1){

       pthread_mutex_lock(&em);

         erro = TRef - T; 
         P = Kp * erro; 
         integ = integ + (erro * Ki);
         Qt = P + integ;
         Qi = Ni * S * (Ti - T);
         Qe = (T - Ta) / R;
         Q = Qt + Qe - Qi;

       pthread_mutex_unlock(&em);
         //resto
   }

   }

   void * exibir_controle(void * arg){ //mostrar na tela sleep 1s
   }

   void * leitura_controle(void * arg){ //aguarda entrada de valores
   }

   void * armazena_arquivo (void * arg) {  //armazena dados de controle 1 e controle 2 em um arquivo
      FILE *file;
      //int result;
      //usar fscanf na saída
      file = fopen("ArqGrav.txt", "w+");
      if (file == NULL){
          printf("Problemas na CRIACAO do arquivo\n");
      }
   }


   void *thread_func(void *arg){
   ptr_thread_arg targ= (ptr_thread_arg)arg;
   int i;

   for (i = targ->idx; i < (targ->idx +targ->lenght); i++){
      printf("Thread %d - value %d\n", (int) pthread_self(), i);
  	}

   }



int main(int argc, char const *argv[]){
         struct timespec t;
        int interval = 50000; /* 50us*/

         while(){
         	

         }

         //------------------
        /* while(1){
         thread_arg arguments[2];
         int i;
         for (int i = 0; i < 2; i++)
         {
            arguments[i].idx = i*10; 
            arguments[i].lenght = 10;

            pthread_create(&(threads[i]), NULL, thread_func, &(arguments[i])); 
            sleep(20);           
         }

         for (int i = 0; i < 2; ++i){
         	pthread_join(threads[i], NULL);
         }
         
         //-----------------
     }*/


        clock_gettime(CLOCK_MONOTONIC ,&t);
        /* start after one second */
        t.tv_sec++;

        while(1) {
                /* wait until next shot */
                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

                /* do the stuff */

                /* calculate next shot */
                t.tv_nsec += interval;

                while (t.tv_nsec >= NSEC_PER_SEC) {
                       t.tv_nsec -= NSEC_PER_SEC;
                        t.tv_sec++;
                }
   }
}