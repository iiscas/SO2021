#include <fcntl.h>	
#include <time.h>	
#include<stdio.h>	
#include<stdlib.h>	
#include<unistd.h>	
#include<string.h>	
#include<sys/wait.h>	
#include<sys/stat.h>	
#include<sys/types.h>	
#include<sys/select.h>	
#include<sys/time.h>	
#include<signal.h>	
#include <errno.h>	

int maxplayer = 30;	
char gamedir[60]=".. \\jogos\\";	
#define MAX 80	
#define MAXPLAYER maxplayer	
#define GAMEDIR gamedir	
int duracao;	
int tempo_espera;	

//nome do fifo de cada cliente	
#define FIFO_CLI "CLI%d"	
//nome do fifo do servidor	
#define FIFO_SERV "SERV"	


// PEDIDO CLIENTE -> SERVIDOR	
typedef struct  {	
    pid_t pid_cliente;	
    char nome[MAX];	
    char jogoAtribuido[MAX]; 	
    char cmd[80];	
}Cliente;	

//RESPOSTA SERVIDOR -> CLIENTE	
typedef struct{	
    int nJogos;	
    char cmd[80];	
    char jogos[2];	
    int nClientesAtivos;	
    Cliente num_jogadores[2];	
}Servidor;	
