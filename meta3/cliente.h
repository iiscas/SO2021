#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>

#define MAX 80
//nome do fifo de cada cliente
#define FIFO_CLI "CLI%d"
//nome do fifo do servidor
#define FIFO_SERV "SERV"

// PEDIDO CLIENTE -> SERVIDOR
typedef struct
{
    pid_t pid_cliente;
    char nome[MAX];
    char jogoAtribuido[MAX];
    char cmd[MAX];
} Cliente;

//RESPOSTA SERVIDOR -> CLIENTE
typedef struct
{
    int nJogos;
    char cmd[MAX];
    char jogos[2];
    int nClientesAtivos;
    Cliente num_jogadores[30];
} Servidor;
