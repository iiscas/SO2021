#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

int pontos = 0;
pid_t pid;
int FLAG_TERMINA = 0;

void atendeSinal(int signal)
{
	fprintf(stderr,"\nSINAL %d RECEBIDO... VAI TERMINAR...", signal);
	fflush(stderr);
	FLAG_TERMINA = 1;
	exit(pontos);
}

int main()
{
	int prog,i,x;
	char escolha[2048];

	//printf("PID: %d\n", getpid());
	
	//signal(SIGUSR1, atendeSinal);
	if (signal(SIGUSR1, atendeSinal) == SIG_ERR)
	{
		perror("\n[ERRO] Não foi possivel configurar o sinal SIGUSR1\n");
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr, "\nSINAL CONFIGURADO\n");
	//fflush(stdout);
	printf("\n-----BEM VINDO------\n---JOGO ADIVINHA O NUMERO---\nO JOGADOR GANHA CASO ACERTE\nNO NUMERO QUE O COMPUTADOR PENSOU!\n");
	//fflush(stdout);
	do
	{
		printf("\nDIGITE UM NUMERO ENTRE 0 E 2: ");
		fflush(stdout);
		scanf("%s",escolha);
		printf("\nRECEBEU ISTO %s\n",escolha);
		//fflush(stdout);
		//x=atoi(escolha);
		if ((strcmp(escolha,"0")==0)  || (strcmp(escolha,"1")==0) || (strcmp(escolha,"2")==0) )
		{

			srand((unsigned)time(NULL));
			prog = rand() % 2;

			if (x == prog)
			{
				printf("Jogador ganhou!!! \n");
				pontos++;
			}

			else
			{
				printf("Jogador perdeu!!! \n");
			}

			printf("O num do computador escolhido foi:  %d \n", prog);
			printf("\nO num de pontos e: %d", pontos);
			printf("\n");
			sleep(1);
		}

	} while (1);
}