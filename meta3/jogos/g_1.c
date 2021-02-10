#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int pontos = 0;
int FLAG_TERMINA = 0;

void handlerSIG(int sig)
{
	printf("\nSINAL %d RECEBIDO... VAI TERMINAR...", signal);
	FLAG_TERMINA = 1;
	exit(pontos);
}

int main()
{
	int prog, jogador, escolha, resultado, i;

	if (signal(SIGUSR1, handlerSIG) == SIG_ERR)
	{
		perror("\n[ERRO] Não foi possivel configurar o sinal SIGUSR1\n");
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr,"\nSINAL CONFIGURADO\n");

	printf("----- BEM VINDO ------\n--- JOGO ADIVINHA O NUMERO ---\n O JOGADOR GANHA CASO A SOMA DOS NUMEROS\n SEJA PAR/IMPAR CONSOANTE O QUE ESCOLHEU!\n----------------------------------\n");

	do
	{
		printf("\nESCOLHA PAR(0) OU ÍMPAR(1): ");
		fflush(stdout);
		scanf("%d", &escolha);

		if (escolha == 0 || escolha == 1)
		{
			printf("\nESCOLHA A SUA JOGADA: ");
			fflush(stdout);
			scanf("%d", &jogador);

			srand((unsigned)time(NULL));
			prog = rand() % 100; // atribui um valor até 10 ao programa com a função rand

			if ((jogador + prog) % 2 == 0)
			{
				resultado = 0;
			}
			else
			{
				resultado = 1;
			}

			if (escolha == resultado)
			{
				printf("JOGADOR GANHOU!!! \n");
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
			printf(" -- NUMERO SO PODE SER O OU 1 !\n");
		}

	} while (FLAG_TERMINA != 1);
}