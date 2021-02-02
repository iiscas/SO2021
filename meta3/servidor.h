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
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#define NTHREAD 2


int maxplayer = 30;
char gamedir[60]="./jogos/";
#define MAX 80
#define MAXPLAYER maxplayer
#define GAMEDIR gamedir



//nome do fifo de cada cliente
#define FIFO_CLI "CLI%d"
//nome do fifo do servidor
#define FIFO_SERV "SERV"
#define FIFO_ANON "ANON"



// PEDIDO CLIENTE -> SERVIDOR
typedef struct  {
    pid_t pid_cliente;
    char nome[MAX];
    char jogoAtribuido[MAX]; 
    char cmd[80];
}Cliente;
