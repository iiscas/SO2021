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
#include <dirent.h>
#include <pthread.h>

int nthread;
#define NTHREAD nthread
#define MAX 80
//nome do fifo de cada cliente
#define FIFO_CLI "CLI%d"
//nome do fifo do servidor
#define FIFO_SERV "SERV"
#define FIFO_ANON "ANON"
int FLAG_SHUTDOWN = 0;
int FLAG_ADICIONA = 0;
int FLAG_CAMPEONATO=0;
int fd_ser, fd_cli;

// PEDIDO CLIENTE -> SERVIDOR
typedef struct  {
    pid_t pid_cliente;
    char nome[MAX];
    char cmd[MAX];
}Cliente;

//RESPOSTA SERVIDOR -> CLIENTE
typedef struct{
    int nJogos;
    char cmd[80];
    char jogos[2];
   // int nClientesAtivos;
    Cliente jogador;
}Servidor;