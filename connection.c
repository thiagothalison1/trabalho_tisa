/*  Programa de demonstração de uso de sockets UDP em C no Linux
 *  Funcionamento:
 *  O programa cliente envia uma msg para o servidor. Essa msg é uma palavra.
 *  O servidor envia outra palavra como resposta.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "connection.h"

#define	TAM_MEU_BUFFER	1000

#define FALHA 1

/**
 * Constants
 */
char* AIR_TEMPERATURE = "sta0";
char* WATER_TEMPERATURE = "st-0";
char* WATER_IN_TEMPERATURE = "sti0";
char* WATER_OUT_FLOW = "sno0";
char* WATER_LEVEL = "sh-0";
char* WATER_IN_FLOW = "ani";
char* HEAT_FLOW = "aq-";

/**
 * Global Variables
 */
int socket_local;
struct sockaddr_in endereco_destino;
pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

int str_cut(char *str, int begin, int len)
{
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return len;
}

int cria_socket_local(void)
{
	socket_local = socket( PF_INET, SOCK_DGRAM, 0);
	if (socket_local < 0) {
		perror("socket");
		return 1;
	}
	return 0;
}

struct sockaddr_in cria_endereco_destino(char *destino, int porta_destino)
{
	struct sockaddr_in servidor; 	/* Endereço do servidor incluindo ip e porta */
	struct hostent *dest_internet;	/* Endereço destino em formato próprio */
	struct in_addr dest_ip;		/* Endereço destino em formato ip numérico */

	if (inet_aton ( destino, &dest_ip ))
		dest_internet = gethostbyaddr((char *)&dest_ip, sizeof(dest_ip), AF_INET);
	else
		dest_internet = gethostbyname(destino);

	if (dest_internet == NULL) {
		fprintf(stderr,"Endereço de rede inválido\n");
		exit(FALHA);
	}

	memset((char *) &servidor, 0, sizeof(servidor));
	memcpy(&servidor.sin_addr, dest_internet->h_addr_list[0], sizeof(servidor.sin_addr));
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(porta_destino);

	return servidor;
}

void envia(char *mensagem)
{
	/* Envia msg ao servidor */
	if (sendto(socket_local, mensagem, strlen(mensagem)+1, 0, (struct sockaddr *) &endereco_destino, sizeof(endereco_destino)) < 0 )
	{ 
		perror("sendto");
		return;
	}
}

int recebe(char *buffer)
{
	int bytes_recebidos;		/* Número de bytes recebidos */

	/* Espera pela msg de resposta do servidor */
	bytes_recebidos = recvfrom(socket_local, buffer, TAM_MEU_BUFFER, 0, NULL, 0);
	if (bytes_recebidos < 0)
	{
		perror("recvfrom");
	}

	return bytes_recebidos;
}

void openConnection(char *host, int port) {
	cria_socket_local();
	endereco_destino = cria_endereco_destino(host, port);
}

void sendMessage(char *message, char *buffer)
{
	char msg_enviada[1000];

	sprintf(msg_enviada, "%s", message);
	envia(msg_enviada);
	int nrec = recebe(buffer);
	buffer[TAM_MEU_BUFFER-1] = '\0';
}

void cut (char *src,  char *dest, int begin) {
	int destIndex = 0;

	for (int i = begin; i < strlen(src); i++) {
		printf("Copiando: %d	%d", destIndex, i);
		dest[destIndex] = src[i];
		destIndex++;
	}
}

void getBoilerInfo(struct boiler_info *info)
{
	pthread_mutex_lock(&socket_mutex);
		char msg_recebida[TAM_MEU_BUFFER];

		sendMessage(AIR_TEMPERATURE, msg_recebida);
		double airTempValueDouble = atof(&msg_recebida[3]);
		info->airTemp = airTempValueDouble;

		sendMessage(WATER_TEMPERATURE, msg_recebida);
		double waterTempValueDouble = atof(&msg_recebida[3]);
		info->waterTemp = waterTempValueDouble;

		sendMessage(WATER_IN_TEMPERATURE, msg_recebida);
		double waterInTempValueDouble = atof(&msg_recebida[3]);
		info->waterInTemp = waterInTempValueDouble;

		sendMessage(WATER_OUT_FLOW, msg_recebida);
		double waterOutFlowValueDouble = atof(&msg_recebida[3]);
		info->waterOutFlow = waterOutFlowValueDouble;

		sendMessage(WATER_LEVEL, msg_recebida);
		double waterLevelValueDouble = atof(&msg_recebida[3]);
		info->waterLevel = waterLevelValueDouble;
	pthread_mutex_unlock(&socket_mutex);

	/* For debugging */
	//printf("T:%f	H:%f\n", info->waterTemp, info->waterLevel);
}

void setBoilerControl(char *controlType, double value)
{
	pthread_mutex_lock(&socket_mutex);
		char msg_recebida[TAM_MEU_BUFFER];
		char message[500];
		sprintf(message, "%s%.1lf", controlType, value);
		sendMessage(message, msg_recebida);
	pthread_mutex_unlock(&socket_mutex);
}
