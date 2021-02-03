#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<signal.h>
#include<time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int pontos=0;
int FLAG_TERMINA=0;


void atendeSinal(int signal){
	printf("\nSINAL %d RECEBIDO... VAI TERMINAR...",signal);
	FLAG_TERMINA=1;
	exit(pontos);
	}

int main()
{
	int prog, jogador, escolha, resultado,i;
	//printf("PID: %d\n",getpid());
	signal(SIGUSR1,atendeSinal);
	if (signal(SIGUSR1,atendeSinal) == SIG_ERR) {
		perror("\n[ERRO] Não foi possivel configurar o sinal SIGUSR1\n");
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr,"\nSINAL CONFIGURADO\n");

	fprintf(stderr,"-------------------------------");
	fprintf(stderr,"\n-----BEM VINDO AO JOGO------\n");
	fprintf(stderr,"---Jogo PAR ou IMPAR---\n");
	fprintf(stderr,"\nO jogador ganha caso a soma dos numeros\nseja par/impar consoante o que escolheu!\n");
	
	do
	{
		fprintf(stderr,"\nEscolha par(0) ou ímpar(1): ");
		fflush(stdout);
		scanf("%d", &escolha);

		if (escolha == 0 || escolha == 1)
		{
			fprintf(stderr,"\nEscolha a sua jogada: ");
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
				fprintf(stderr,"Jogador ganhou!!! \n");
				pontos++;
			}

			else
			{
				fprintf(stderr,"Jogador perdeu!!! \n");
			}

			printf("O num do computador escolhido foi:  %d \n", prog);
			printf("\nO num de pontos e: %d", pontos);
			printf("\n");
			sleep(1);
		}

	}while(FLAG_TERMINA!=1);

}