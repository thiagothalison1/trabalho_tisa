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
	int socket_local;		/* Socket usado na comunicacão */

	socket_local = socket( PF_INET, SOCK_DGRAM, 0);
	if (socket_local < 0) {
		perror("socket");
		return 1;
	}
	return socket_local;
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
	socket_local = cria_socket_local();
	endereco_destino = cria_endereco_destino(host, port);
}

char* sendMessage(char *message)
{
	char msg_enviada[1000];
	char *msg_recebida = malloc (sizeof (char) * TAM_MEU_BUFFER);

	sprintf(msg_enviada, "%s", message);
	envia(msg_enviada);
	int nrec = recebe(msg_recebida);
	return msg_recebida;
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
	char *airTemp = sendMessage(AIR_TEMPERATURE);
	char *airTempValue = (char*) malloc(6);
	strncpy(airTempValue, airTemp + 3, strlen(airTemp));
	double airTempValueDouble = atof(airTempValue);
	info->airTemp = airTempValueDouble;

	char *waterTemp = sendMessage(WATER_TEMPERATURE);
	char *waterTempValue = (char*) malloc(6); 
	strncpy(waterTempValue, waterTemp + 3, strlen(waterTemp));
	double waterTempValueDouble = atof(waterTempValue);
	info->waterTemp = waterTempValueDouble;

	char *waterInTemp = sendMessage(WATER_IN_TEMPERATURE);
	char *waterInTempValue = (char*) malloc(6);
	strncpy(waterInTempValue, waterInTemp + 3, strlen(waterInTemp));
	double waterInTempValueDouble = atof(waterInTempValue);
    info->waterInTemp = waterInTempValueDouble;

	char *waterOutFlow = sendMessage(WATER_OUT_FLOW);
	char *waterOutFlowValue = (char*) malloc(6);
	strncpy(waterOutFlowValue, waterOutFlow + 3, strlen(waterOutFlow));
	double waterOutFlowValueDouble = atof(waterOutFlowValue);
    info->waterOutFlow = waterOutFlowValueDouble;

	char *waterLevel = sendMessage(WATER_LEVEL);
	char *waterLevelValue = (char*) malloc(6);
	strncpy(waterLevelValue, waterLevel + 3, strlen(waterLevel));
	double waterLevelValueDouble = atof(waterLevelValue);
    info->waterLevel = waterLevelValueDouble;
}

char* setBoilerControl(char *controlType, double value)
{
	char message[500];
	sprintf(message, "%s%.1lf", controlType, value);
	//printf("%s", message);
	return sendMessage(message);
}


// int main(int argc, char *argv[])
// {
// 	if (argc < 4) { 
// 		fprintf(stderr,"Uso: udpcliente endereço porta palavra \n");
// 		fprintf(stderr,"onde o endereço é o endereço do servidor \n");
// 		fprintf(stderr,"porta é o número da porta do servidor \n");
// 		fprintf(stderr,"palavra é a palavra que será enviada ao servidor \n");
// 		fprintf(stderr,"exemplo de uso:\n");
// 		fprintf(stderr,"   udpcliente baker.das.ufsc.br 1234 \"ola\"\n");
// 		exit(FALHA);
// 	}

// 	char *host = argv[1];
// 	int porta = atoi(argv[2]);
// 	openConnection(host, porta);

// 	char *mensagem = argv[3];
// 	char *mensagemRecebida = sendMessage(mensagem);

// 	printf("Resposta: %s \n", mensagemRecebida);


// 	// int socket_local = cria_socket_local();

// 	// struct sockaddr_in endereco_destino = cria_endereco_destino(argv[1], porta_destino);

// 	//int i = 0;    
// 	//char msg_enviada[1000];  
// 	//char msg_recebida[1000];
// 	//int nrec;

// 	//do{
// 		//printf("tentativa %d\n", i);
// 		//sprintf(msg_enviada, "tentativa %d ", i);
// 		//sprintf(msg_enviada, "%s", argv[3]);
// 		//strcat(msg_enviada, argv[3]);

// 		//envia_mensagem(socket_local, endereco_destino, msg_enviada);

// 		//nrec = recebe_mensagem(socket_local, msg_recebida, 1000);
		
// 		//double temp = strtod(msg_recebida, NULL);
		
// 		//printf("Mensagem de resposta com %d bytes %s \n", nrec, msg_recebida);

// 		//sleep(1);
// 		//++i;

// 	//} while( i < 5000 );

// }
