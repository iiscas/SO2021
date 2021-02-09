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

int nthread;
#define MAXPLAYER 10
#define NTHREAD nthread
#define MAX 2048
//nome do fifo de cada cliente
#define FIFO_CLI "CLI%d"
//nome do fifo do servidor
#define FIFO_SERV "SERV"
#define FIFO_ANON "ANON"
int FLAG_SHUTDOWN = 0;
int FLAG_ADICIONA = 0;
int FLAG_CAMPEONATO = 0;
int fd_ser, fd_cli;

// PEDIDO CLIENTE -> SERVIDOR
typedef struct
{
    int pid_cliente;
    char nome[MAX];
    char cmd[MAX];
    int acesso; // ver se é a primeira msg
    char jogo[MAX];
} Cliente;


//RESPOSTA SERVIDOR -> CLIENTE
typedef struct
{
    int nJogos;
    char cmd[MAX];
    char jogoAtribuido[MAX];
    int p[2],r[2];
    int pid_jogo;
    int avanca;
    Cliente jogador;
} Servidor;