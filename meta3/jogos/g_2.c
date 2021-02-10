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
	fprintf(stderr, "\nSINAL %d RECEBIDO... VAI TERMINAR...", signal);
	fflush(stderr);
	FLAG_TERMINA = 1;
	exit(pontos);
}

int main()
{
	int prog, i, x;
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
	printf("\n----- BEM VINDO ------\n--- JOGO ADIVINHA O NUMERO ---\n O JOGADOR GANHA CASO ACERTE\n NO NUMERO QUE O COMPUTADOR PENSOU!\n----------------------------------\n");
	//fflush(stdout);
	do
	{
		printf("\nDIGITE UM NUMERO ENTRE 0 E 2: ");
		fflush(stdout);
		scanf("%s", escolha);
		//printf("\nRECEBEU ISTO %s\n",escolha);
		fflush(stdout);
		//x=atoi(escolha);
		if ((strcmp(escolha, "0") == 0) || (strcmp(escolha, "1") == 0) || (strcmp(escolha, "2") == 0))
		{
			x = atoi(escolha);
			srand((unsigned)time(NULL));
			prog = rand() % 2;

			if (x == prog)
			{
				printf("JOGADOR GANHOU!!!\n");
				pontos++;
			}

			else
			{
				printf("JOGADOR PERDEU!!! \n");
			}

			printf("O NUMERO ALEATORIO FOI:  %d \n", prog);
			printf("PONTUACAO:%d", pontos);
			printf("\n");
			sleep(1);
		}

		else
		{
			printf("-- NUMERO SO PODE SER 0, 1 OU 2 !\n");
		}
	} while (1);
}