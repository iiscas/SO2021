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
	int prog, escolha, i;

	printf("PID: %d\n", getpid());
	fflush(stdout);
	//signal(SIGUSR1, atendeSinal);
	if (signal(SIGUSR1, atendeSinal) == SIG_ERR)
	{
		perror("\n[ERRO] Não foi possivel configurar o sinal SIGUSR1\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "\nSINAL CONFIGURADO\n");

	printf("-----BEM VINDO------\n");
	printf("---Jogo Adivinha o Numero---\n");
	printf("\nO jogador ganha caso acerte\nno numero que o computador 'pensou'!\n");

	do
	{
		printf("\nDigite um numero entre 0 e 10\n");
		scanf("%d", &escolha);

		if (escolha >= 0 && escolha <= 2)
		{

			srand((unsigned)time(NULL));
			prog = rand() % 2;

			if (escolha == prog)
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

	} while (FLAG_TERMINA != 1);
}