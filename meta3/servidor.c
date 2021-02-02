#include "servidor.h"

int fd_ser, fd_cli;

#define NTHREAD 2

typedef struct{
	char letra;
	char continua;
	pthread_mutex_t *trinco;
}TDATA;
int FLAG_SHUTDOWN = 0;

void findJogos()
{
    DIR *folder;
    struct dirent *entry;
    int files = 0;

    folder = opendir("./jogos");
    if (folder == NULL)
    {
        perror("Unable to read directory");
        //return(1);
    }

    while ((entry = readdir(folder)))
    {
        files++;
        if ((strcmp(entry->d_name, "g_1.c") == 0) || (strcmp(entry->d_name, "g_2.c") == 0))
        {
            printf(" - %s\n",
                   entry->d_name);
        }
    }

    closedir(folder);
}
void randPorFuncao()
{
    srand(time(NULL));
}
void trataSig(int i)
{
    fprintf(stderr, "\nSERVIDOR A TERMINAR VIA TECLADO.....\n");
    remove(FIFO_SERV);
    close(fd_ser);
    unlink(FIFO_SERV);
    exit(EXIT_SUCCESS);
}
void eliminaCliente(int pid, Servidor *s)
{
    int i;
    for (i = 0; i < s->nClientesAtivos; i++)
    {
        if (pid == s->num_jogadores[i].pid_cliente)
        {
            printf("ADEUS %s ....\n", s->num_jogadores[i].nome);
            s->num_jogadores[i] = s->num_jogadores[s->nClientesAtivos - 1];
            s->nClientesAtivos--;
            printf("\nNR DE CLIENTES AINDA ATIVOS: %d\n", s->nClientesAtivos);
        }
    }
}
int existeCliente(char nome[], Servidor s)
{
    int i;
    for (i = 0; i < s.nClientesAtivos; i++)
    {
        if (strcmp(s.num_jogadores[i].nome, nome) == 0)
            return 1;
    }
    return 0;
}
int adicionaCLiente(Cliente a, Servidor *s)
{
    if (existeCliente(a.nome, *s))
    {
        return 0;
    }
    else
    {

        s->num_jogadores[s->nClientesAtivos] = a;
        s->nJogos = rand() % 2;
        //        printf("\nNUMERO RANDOM: %d\n", s.nJogos);

        if (s->nJogos == 0)
        {
            strcpy(a.jogoAtribuido, "g_1");
            strcpy(s->num_jogadores[s->nClientesAtivos].jogoAtribuido, "g_1.c");
        }
        else
        {
            strcpy(a.jogoAtribuido, "g_2");
            strcpy(s->num_jogadores[s->nClientesAtivos].jogoAtribuido, "g_2.c");
        }
       // printf("Adicionei jogador %s --  comandos: %s -- jogo: %s\n", s->num_jogadores[s->nClientesAtivos].nome, s->cmd, s->num_jogadores[s->nClientesAtivos].jogoAtribuido);
        s->nClientesAtivos++;
        return 1;
    }
}
/* void trataCliente(Servidor s, Cliente c, char fifo_name, int r)
{
    randPorFuncao();
    adicionaCLiente(c, s);
    fprintf(stderr, "\n%s INICIOU SESSAO\n", c.nome);

    //strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);
    //printf("\n%s", c.jogoAtribuido);

    sprintf(fifo_name, "CLI%d", c.pid_cliente);
    //printf("NOME DO PIPE: %s\n", fifo_name);
    fd_cli = open(fifo_name, O_WRONLY);
    r = write(fd_cli, &c, sizeof(Cliente));

    close(fd_cli);
    //printf("ENVIEI ISTO %s", c.jogoAtribuido);
} */
void players(Servidor s)
{
    if (s.nClientesAtivos == 0)
        printf("\nNAO HA JOGADORES LIGADOS!\n");
    else
    {
        printf("\n==== JOGADORES NO CAMPEONATO ====\n");
        for (int i = 0; i < s.nClientesAtivos; i++)
        {

            printf(" Jogador %s --- Jogo atribuido: %s \n", s.num_jogadores[i].nome, s.num_jogadores[i].jogoAtribuido);
        }
    }
}
void games()
{
    printf("\n---------------------\n");
    printf("   Jogos disponiveis:");
    printf("\n---------------------\n");
    findJogos();
}
void settings(int duracao, int tempo_espera)
{
    printf("\n=======================================");
    printf("\n======= BEM VINDO AO CHAMPION =========\n");
    printf("=======================================\n");

    printf("\n------- Configuracoes Iniciais --------\n");
    printf("\no Duracao do campeonato: %d s\n", duracao);
    printf("o Valor de tempo de espera: %d s\n", tempo_espera);
    printf("o Numero maximo de jogadores: %d\n", MAXPLAYER);
}
void comandosMenu()
{

    printf("\n------ Comandos Disponiveis ------------\n");
    printf("o Listar jogadores e jogo <players>\n");
    printf("o Listar jogos disponíveis <games>\n");
    printf("o Remover um certo jogador <k(nome)>\n");
    printf("o (Suspender comunicacao entre jogador e jogo <s(nome_jogador)>)\n");
    printf("o (Retomar comunicacao entre jogador e jogo <r(nome_jogador)>)\n");
    printf("o (Encerrar campeonato <end>)\n");
    printf("o Sair encerrando o arbitro <exit>\n");
    printf("\n---------------------------------------\n");
}
/* typedef struct
{
    int jogo;
    pthread_mutex_t *trinco;
} TDATA; */
int p[2],r[2];
int filho;
int estado;
void iniciaJogo(){
    int estado,num;

    pipe(p);
    pipe(r);
    filho=fork();
    if(filho==0){
      close(0);//FECHAR ACESSO AO TECLADO
		dup(p[0]);//DUPLICAR P[0] NA PRIMEIRA POSICAO DISPONIVEL
		close(p[0]);//FECHAR EXTREMIDADE DE LEITURA DO PIPE
		close(p[1]);//FECHAR EXTREMIDADE DE ESCRITA DO PIPE

		/* close(1);//FECHAR ACESSO AO MONITOR
		dup(r[1]);//DUPLICAR P[1] NA PRIMEIRA POSICAO DISPONIVEL
		close(r[0]);//FECHAR EXTREMIDADE DE LEITURA DO PIPE
		close(r[1]);//FECHAR EXTREMIDADE DE ESCRITA DO PIPE */
		execl("./jogos/g_1", "g_1", NULL);
	}
	close(p[0]);
	close(r[1]);
    wait(&estado);
}
void *mostra (void *dados){
	int i, *res;
	TDATA *pdata;

	pdata=(TDATA*) dados;
	res=(int*) malloc(sizeof(int));
	res=0;
	do{
		for(i=0; i<rand()%5; i++){
			printf("."); 
			fflush(stdout);
			sleep(1);
		} 
		pthread_mutex_lock(pdata->trinco);
		/*Mostra* PEDIDO (INFORMACAO) no monitor*/

		pthread_mutex_unlock(pdata->trinco);
		(*res)++;
		for(i=0; i<3; i++){
			printf("%c", pdata->letra); 
			fflush(stdout);
		   	sleep(1);
		}
		
	}while(pdata->continua);
	res=(int*) malloc(sizeof(int));
	*res=123;
	pthread_exit((void*) res);

}







int main(int argc, char *argv[])
{
    Cliente c;
    Servidor s;
    int duracao, tempo_espera, i = 0, res, r, fds_anon, canal[2], res_canal, n, estado;
    char *ptr, *ptr1, *ptr2, fifo_name[20], cmd[90];
    s.nClientesAtivos = 0;
    fd_set fds;
    struct timeval tempo;

    if (signal(SIGINT, trataSig) == SIG_ERR)
    {
        perror("\n NAO FOI POSSIVEL SINAL SIGNINT\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\nSINAL SIGNIT CONFIGURADO\n");

    if (access(FIFO_SERV, F_OK) != 0)
    {
        mkfifo(FIFO_SERV, 0600);
        fprintf(stderr, "\nCRIOU O FIFO SERV\n");
    }

    //abrir fifo do servidor
    fd_ser = open(FIFO_SERV, O_RDWR);

    if (argc != 3)
    {
        printf("\n[ERRO] Nr. de args! \n");
        printf("\n--> Deve colocar no seguinte formato: $./servidor x y\n");
        printf("x- duracao do campeonato(s) e y- tempo de espera\n");
        exit(3);
    }

    duracao = strtol(argv[1], &ptr, 10);
    tempo_espera = strtol(argv[2], &ptr1, 10);

    if (getenv("MAXPLAYER") != NULL)
        maxplayer = strtol(getenv("MAXPLAYER"), &ptr2, 10);
    if (getenv("GAMEDIR") != NULL)
        strcpy(gamedir, getenv("GAMEDIR"));

    settings(duracao, MAXPLAYER);
    comandosMenu();
   // iniciaJogo();
char str[40];
	int *resultado;
	TDATA tinfo[NTHREAD];
	pthread_t tarefa[NTHREAD];
	pthread_mutex_t trinco;

	printf("PID: %d\n", getpid());

	srand((unsigned int) time(NULL));
	pthread_mutex_init(&trinco, NULL);
	for(i=0; i<NTHREAD; i++){
		tinfo[i].letra='A'+ i;
		tinfo[i].continua=1;
		tinfo[i].trinco=&trinco;
		pthread_create(&tarefa[i], NULL, mostra,(void*) &tinfo[i]);
	}
	do{
		printf("COMANDO: ");
		scanf("%s", str);
		printf("Recebi o comando '%s'...\n", str);	
	}while(strcmp(str, "sair")!=0);

	for(i=0; i<NTHREAD; i++){
		printf("Vou pedir a thread %d para terminar", i); fflush(stdout);
		tinfo[i].continua=0;
		pthread_join(tarefa[i], (void*) &resultado);
		printf("....terminou! (%d)\n", *resultado);
		free(resultado);
	}
	pthread_mutex_destroy(&trinco);
	exit(0);








    do
    {
        fflush(stdout);
        printf("\nINTRODUZA UM COMANDO: ");
        fflush(stdout);

        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(fd_ser, &fds);

        tempo.tv_sec = tempo_espera; //vai buscar valor pela linha de comandos
        tempo.tv_usec = 0;
        res = select(fd_ser + 1, &fds, NULL, NULL, &tempo);

        if (res == 0)
        {
            printf("NAO HA DADOS...\n");
        }

        else if (res > 0 && FD_ISSET(0, &fds)) //receber do teclado
        {
            //ler comandos do admin
            scanf("%s", cmd); // ha dados... nao bloqueia
            printf("O ADMIN PEDIU O COMANDO '%s' ...\n", cmd);
            fflush(stdout);

            if (strcmp(cmd, "players") == 0)
            {
                players(s);
            }

            else if (cmd[0] == 'k')
            {
                for (int i = 0; i < s.nClientesAtivos; i++)
                {

                    if (strcmp(memmove(cmd, cmd + 1, strlen(cmd)), s.num_jogadores[i].nome) == 0)
                    {
                        strcpy(c.jogoAtribuido, "quit");

                        sprintf(fifo_name, FIFO_CLI, s.num_jogadores[i].pid_cliente);
                        printf("NOME DO PIPE: %s\n", fifo_name);
                        fd_cli = open(fifo_name, O_WRONLY);
                        r = write(fd_cli, &c, sizeof(Cliente));

                        close(fd_cli);
                        eliminaCliente(s.num_jogadores[i].pid_cliente, &s);
                    }
                }
            }

            else if (strcmp(cmd, "games") == 0)
            {
                games();
            }

            else if (strcmp(cmd, "exit") == 0)
            {
                FLAG_SHUTDOWN = 1;
            }
            else
            {
                printf("\nCOMANDO INVALIDO!\n");
            }
        }

        else if (res > 0 && FD_ISSET(fd_ser, &fds)) //recber do fifo
        {

            r = read(fd_ser, &c, sizeof(Cliente)); //abre o que o fd_ser enviou
            //printf("\nRECEBI DO FIFO %d ///%s %s %s \n", c.pid_cliente, c.nome, c.cmd, c.jogoAtribuido);

            if (r == sizeof(Cliente))
            {
                if (existeCliente(c.nome, s) == 1)
                {
                    if (strcmp(c.cmd, "#mygame") == 0)
                    {

                        for (int i = 0; i < s.nClientesAtivos; i++)
                        {
                            if (c.pid_cliente == s.num_jogadores[i].pid_cliente)
                            {

                                strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);

                                printf("aqui %s", c.jogoAtribuido);

                                sprintf(fifo_name, FIFO_CLI, c.pid_cliente);
                                //printf("NOME DO PIPE: %s\n", fifo_name);
                                fd_cli = open(fifo_name, O_WRONLY);
                                r = write(fd_cli, &c, sizeof(Cliente));

                                close(fd_cli);
                                //printf("ENVIEI ISTO %s", c.jogoAtribuido);
                            }
                        }
                    }
                    else if (strcmp(c.cmd, "#quit") == 0)
                    {
                        for (int i = 0; i < s.nClientesAtivos; i++)
                        {
                            if (c.pid_cliente == s.num_jogadores[i].pid_cliente)
                            {
                                printf("%s pretende desistir do campeonato!\n", s.num_jogadores[i].nome);
                                strcpy(c.jogoAtribuido, "quit");

                                eliminaCliente(s.num_jogadores[i].pid_cliente, &s);
                                sprintf(fifo_name, FIFO_CLI, c.pid_cliente);
                                //printf("NOME DO PIPE: %s\n", fifo_name);
                                fd_cli = open(fifo_name, O_WRONLY);
                                r = write(fd_cli, &c, sizeof(Cliente));

                                close(fd_cli);
                                //printf("ENVIEI ISTO %s", c.jogoAtribuido);
                            }
                        }
                    }
                }

                else if (s.nClientesAtivos < 2)
                {
                    randPorFuncao();
                    adicionaCLiente(c, &s);
                    fprintf(stderr, "\n%s INICIOU SESSAO\n", c.nome);

                    strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);

                    sprintf(fifo_name, "CLI%d", c.pid_cliente);
                    fd_cli = open(fifo_name, O_WRONLY);
                    r = write(fd_cli, &c, sizeof(Cliente));

                    close(fd_cli);
                }
            }
        }

    } while (FLAG_SHUTDOWN != 1);


    printf("A TERMINAR O SERVIDOR....ADEUS...\n");
    sleep(1);

    remove(FIFO_SERV);
    close(fd_ser);
    unlink(FIFO_SERV);
    return EXIT_SUCCESS;
}