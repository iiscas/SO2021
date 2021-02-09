#include "servidor.h"
char *maxplayer;
char *gamedir;
Cliente c;
Servidor s[MAXPLAYER];

int res;
int nClientesAtivos = 0;
int i = 0;
int temporizador = 0;
int tempo_espera, duracao;
int findJogos(char *gd)
{
    DIR *folder;
    struct dirent *dir;
    int files = 0;

    folder = opendir(gd);
    if (folder == NULL)
    {
        perror("Unable to read directory");
        //return(1);
    }
    if (folder)
    {
        while ((dir = readdir(folder)) != NULL)
        {

            if (dir->d_name[0] == 'g' && dir->d_name[strlen(dir->d_name) - 1] == 'c')
            {
                printf(" - %s\n", dir->d_name);
                files++;
            }
        }
        closedir(folder);
    }
    return files;
}
void randomJogo(Servidor *s)
{
    srand(time(NULL));
    DIR *jogo;
    struct dirent *dirJogos;
    int jogoAtual = 1;
    int jogoRandom;
    int nJogos = findJogos(gamedir);

    //NUMERO RANDOM ENTRE 1 O NUMERO DE FILES LIDOS
    jogoRandom = rand() % nJogos + 1;

    jogo = opendir(gamedir);
    if (jogo)
    {
        while ((dirJogos = readdir(jogo)))
        {
            if ((strcmp(dirJogos->d_name, "g_1.c") == 0) || (strcmp(dirJogos->d_name, "g_2.c") == 0))
            {

                // printf("Jogo Atual: %d\n", jogoAtual);

                if (jogoRandom == jogoAtual)
                {

                    //printf("Nome do jogo a devolver: %s\n", dirJogos->d_name);
                    dirJogos->d_name[strlen(dirJogos->d_name) - 2] = '\0';
                    strcpy(s[nClientesAtivos].jogador.jogo, dirJogos->d_name);
                    //return (dirJogos->d_name);
                }

                jogoAtual++;
            }
        }
        closedir(jogo);
    }
}

void comecarCamp()
{
    printf("\n\n=======================================");
    printf("\n======= CAMPEONATO VAI COMECAR ========\n");
    printf("=======================================\n");
    FLAG_ADICIONA = 1;
}
void settings(int duracao, int tempo_espera)
{
    maxplayer = getenv("MAXPLAYER");
    gamedir = getenv("GAMEDIR");

    printf("\n=======================================");
    printf("\n======= BEM VINDO AO CHAMPION =========\n");
    printf("=======================================\n");
    printf("\n------- Configuracoes Iniciais --------\n");
    printf("\no Duracao do campeonato: %d s\n", duracao);
    printf("o Valor de tempo de espera: %d s\n", tempo_espera);
    printf("o Numero maximo de jogadores: %s\n", maxplayer);
    printf("o Diretoria dos jogos: %s\n", gamedir);
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
void trataSig(int i)
{
    fprintf(stderr, "\nSERVIDOR A TERMINAR VIA TECLADO.....\n");
    remove(FIFO_SERV);
    close(fd_ser);
    unlink(FIFO_SERV);
    exit(EXIT_SUCCESS);
}
void eliminaCliente(int pid, Servidor s[])
{
    int i;
    for (i = 0; i < nClientesAtivos; i++)
    {
        if (pid == s[i].jogador.pid_cliente)
        {
            printf("ADEUS %s ....\n", s[i].jogador.nome);
            s[i] = s[nClientesAtivos - 1];
            nClientesAtivos--;
            printf("\nNR DE CLIENTES AINDA ATIVOS: %d\n", nClientesAtivos);
        }
    }
}
int existeCliente(char nome[], Servidor *s)
{
    int i;
    for (i = 0; i < nClientesAtivos; i++)
    {
        if (strcmp(s[i].jogador.nome, nome) == 0)
            return 1;
    }
    return 0;
}

int end()
{
    printf("A TERMINAR O SERVIDOR....ADEUS...\n");
    sleep(1);
    remove(FIFO_SERV);
    close(fd_ser);
    unlink(FIFO_SERV);
    return EXIT_SUCCESS;
}
void *Jogo(void *dados)
{
    Servidor *s;
    s = (Servidor *)dados;
    int p[2], r[2], z = 1;
    char j[50], str[2048], fifo_name[50];
    int resposta, pid_filho;
    char *jogo=s->jogador.jogo;
    char *dir=gamedir;
    char *tmp=strdup(dir);
    strcat(tmp,jogo);
    //printf("DIR -->%s\n",dir);
    printf("TEMP -->%s\n",tmp);
    strcpy(s->jogador.jogo,tmp);
     printf("JOGO -->%s\n",jogo);
     free(tmp);


    pipe(p);
    pipe(r);
    sprintf(fifo_name, "CLI%d", s->jogador.pid_cliente);
    pid_filho = fork();
    if (pid_filho == 0)
    {
        printf("JOGO %s PARA CLIENTE %s COMECOU! \n",s->jogador.jogo, s->jogador.nome);
        //printf("Filho para o %s (PID: %d)\n", s->jogador.nome, s->jogador.pid_cliente);

        close(1);    // fecha o stdout do jogo
        close(r[0]); // fecha o stdin do pipe
        dup(r[1]);   // coloca o stdout do pipe no stdout do jogo
        close(r[1]); // fecha o stdout do pipe (o que já não precisamos de usar)

        close(0);
        close(p[1]);
        dup(p[0]);
        close(p[0]);
        execl(s->jogador.jogo,s->jogador.jogo, NULL);
    }
    else
    {
        close(p[0]);
        close(r[1]);
    }

    while (z != 0)
    {
        while ((resposta = read(r[0], str, sizeof(str))) > 0)
        {
            str[resposta] = '\0';
            strcpy(c.cmd, str);

            if ((fd_cli = open(fifo_name, O_WRONLY)) > 0)
            {
                res = write(fd_cli, &c, sizeof(Cliente));

                close(fd_cli);
                //printf("Erro a abrir o fifo do cliente %s (PID: %d)\n", s->jogador.nome, s->jogador.pid_cliente);
            }

            while (s->avanca == 0)
            {

                sleep(1);
            }
            if (s[i].jogador.cmd[0] != '#')
            {

                write(p[1], s[i].jogador.cmd, strlen(s[i].jogador.cmd)); // escreve no cliente i o numero, será que não está a enviar a estrutura bem?
            }
            s->avanca = 0;
        }

        close(r[0]);
        close(p[1]);
    }
}

void *Campeonato(void *dados)
{
    Servidor *s;
    s = (Servidor *)dados;
    char g[MAX];

    pthread_t jogo;
    printf("O CAMPEONATO VAI COMECAR DAQUI A %d SEGUNDOS\n", tempo_espera);
    sleep(tempo_espera);
    comecarCamp();

    //ATRIBUIR JOGO RANDOM PARA CADA JOGADOR
    for (int i = 0; i < nClientesAtivos; i++)
    {
        /* strcpy(g, randomJogo());
        strcpy(s[i].jogador.jogo, g);*/
        //printf("JOGO ATRIBUIDO: %s",findJogos());
        pthread_create(&jogo, NULL, Jogo, (void *)&s[i]);
        //printf(" Jogador %s --- Jogo atribuido: %s --- PID: %d\n", s[i].jogador.nome, s[i].jogador.jogo, s[i].jogador.pid_cliente);
    }
};
void *ClienteServidor(void *dados)
{
    //fprintf(stderr, "\n Cliente com o PID %d esta a tentar conectar.\n", s.num_jogadores[s.nClientesAtivos].pid_cliente);
    Servidor *s;
    Servidor *currentUser;
    s = (Servidor *)dados;
    //msgForservidor=s
    char fifo_name[50];
    int r;
    char cmd[60];

    //s.nClientesAtivos = 0;
    //THREAD
    pthread_t campeonatoT;

    do
    {
        r = read(fd_ser, &c, sizeof(Cliente)); //abre o fifo que recebeu do cliente

        if (c.acesso == 0)
            fprintf(stderr, "\n CLIENTE %s COM PID %d QUER SE REGISTAR!\n", c.nome, c.pid_cliente);
        else
            fprintf(stderr, "\n COMANDO RECEBIDO DO CLIENTE COM O PID %d: [%s]\n", c.pid_cliente, c.cmd);

        sprintf(fifo_name, "CLI%d", c.pid_cliente);
        //printf("FIFO NAME: %s\n", fifo_name);

        if ((fd_cli = open(fifo_name, O_WRONLY)) < 0)
            perror("\n ERRO AO ABRIR FIFO CLI");

        else
        {
            if (r == sizeof(Cliente))
            {
                if (nClientesAtivos < 30)
                {
                    if (c.acesso == 0)
                    {
                        if (existeCliente(c.nome, s) != 1)
                        {
                            strcpy(s[nClientesAtivos].jogador.nome, c.nome);
                            s[nClientesAtivos].jogador.pid_cliente = c.pid_cliente;
                            randomJogo(s);
                            //strcpy(s[nClientesAtivos].jogador.jogo, "g_2");

                            strcpy(c.cmd, "INICIOU SESSAO, JA PODE INSERIR COMANDOS!\n");
                            nClientesAtivos++;

                            fprintf(stderr, "\n%s INICIOU SESSAO\n", c.nome);
                            //printf("NOME DO PIPE: %s\n", fifo_name);
                            r = write(fd_cli, &c, sizeof(Cliente));

                            if (nClientesAtivos > 1 && temporizador == 0)
                            {
                                temporizador = 1;
                                pthread_create(&campeonatoT, NULL, Campeonato, (void *)s);
                            }
                        }
                        else
                        {
                            kill(c.pid_cliente, SIGINT);
                            nClientesAtivos--;
                        }
                    }

                    if (c.acesso != 0)
                    {
                        //fprintf(stderr, "\nFIFO do cliente %s aberto para escrita.\n", fifo_name);

                        if (strcmp(c.cmd, "#mygame") == 0)
                        {
                            //fprintf(stderr, "\n----FIFO do cliente %s aberto para escrita.\n", fifo_name);
                            for (int i = 0; i < nClientesAtivos; i++)
                            {
                                //printf("PIDS: %d \n", s[i].jogador.pid_cliente);
                                if (c.pid_cliente == s[i].jogador.pid_cliente)
                                {

                                    strcpy(c.jogo, s[i].jogador.jogo);

                                    //printf("JOGO %s \n", c.jogo);

                                    r = write(fd_cli, &c, sizeof(Cliente));
                                }
                            }
                        }
                        else if (strcmp(c.cmd, "#quit") == 0)
                        {
                            for (int i = 0; i < nClientesAtivos; i++)
                            {
                                if (c.pid_cliente == s[i].jogador.pid_cliente)
                                {
                                    printf("\n%s PRETENDE DESISTIR DO CAMPEONATO!\n", s[i].jogador.nome);
                                    strcpy(c.cmd, "#quit");

                                    eliminaCliente(s[i].jogador.pid_cliente, &s[i]);

                                    //printf("NOME DO PIPE: %s\n", fifo_name);

                                    r = write(fd_cli, &c, sizeof(Cliente));
                                }
                            }
                        }
                        else if (c.cmd[0] != '#')
                        {
                            //printf("AQUI!!!!!!\n");
                            for (int i = 0; i < nClientesAtivos; i++)
                            {
                                if (c.pid_cliente == s[i].jogador.pid_cliente)
                                {
                                    c.cmd[strlen(c.cmd) + 1] = '\0';
                                    c.cmd[strlen(c.cmd)] = '\n';

                                    strcpy(s[i].jogador.cmd, c.cmd);
                                    //printf("--->vou mandar isto '%s'\n", s[i].jogador.cmd);
                                    s[i].avanca = 1;
                                    //r = write(s[i].p[1], c.cmd, strlen(c.cmd)); // escreve no cliente i o numero, será que não está a enviar a estrutura bem?
                                }
                            }
                        }
                    }
                }
            }

            else
            {
                printf("ESTRUTURA RECEBIDA COM TAMANHO DIFERENTE!\n");
            }
        }

    } while (1);
    close(fd_cli);
    close(fd_ser);
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    //s.nClientesAtivos = 0;
    int i = 0, res, r, n, estado;
    char *ptr, *ptr1, *ptr2, fifo_name[20], cmd[90];
    char str[40];
    int z, *resultado;
    struct timeval tempo;
    struct sigaction act;
    act.sa_handler = comecarCamp;
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);
    srand(time(NULL));

    //WARNINGS
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

    //ABRIR FIFO DO SERVIDOR
    fd_ser = open(FIFO_SERV, O_RDWR);
    //VERIFICAR ARGUMENTOS PASSADOS PELA LINHA DE COMANDOS
    if (argc != 3)
    {
        printf("\n[ERRO] Nr. de args! \n");
        printf("\n--> Deve colocar no seguinte formato: $./servidor x y\n");
        printf("x- duracao do campeonato(s) e y- tempo de espera\n");
        exit(3);
    }

    //TRATA ARGUMENTOS DA LINHA DE COMANDOS
    duracao = strtol(argv[1], &ptr, 10);
    tempo_espera = strtol(argv[2], &ptr1, 10);

    //IMPRIME CONFIGURAÇÕES E COMANDOS
    settings(duracao, tempo_espera);
    comandosMenu();

    // -- Cria thread para a comunicação entre servidor e cliente  --
    pthread_t ligaCliServ;
    pthread_create(&ligaCliServ, NULL, ClienteServidor, (void *)&s);

    do
    {
        printf("\nINTRODUZ COMANDO: ");
        scanf("%s", cmd);
        //fflush(stdout);

        if (strcmp(cmd, "players") == 0)
        {
            if (nClientesAtivos <= 0)
                printf("\nNAO HA JOGADORES LIGADOS!\n");
            else
            {
                printf("\n==== JOGADORES NO CAMPEONATO ====\n");
                for (int i = 0; i < nClientesAtivos; i++)
                {
                    printf(" Jogador %s --- Jogo atribuido: %s --- PID: %d\n", s[i].jogador.nome, s[i].jogador.jogo, s[i].jogador.pid_cliente);
                }
            }
        }
        else if (strcmp(cmd, "games") == 0)
        {
            printf("\n---------------------\n");
            printf("   Jogos disponiveis:");
            printf("\n---------------------\n");
            findJogos(gamedir);
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            FLAG_SHUTDOWN = 1;
        }
        else if (cmd[0] == 'k')
        {
            char *nome;
            nome = strtok(cmd, "k");

            for (int i = 0; i < nClientesAtivos; i++)
            {
                if (strcmp(s[i].jogador.nome, nome) == 0)
                {
                    kill(s[i].jogador.pid_cliente, SIGINT);
                    printf("ADEUS %s ....\n", s[i].jogador.nome);
                    s[i] = s[nClientesAtivos - 1];
                    nClientesAtivos--;
                    printf("\nNR DE CLIENTES AINDA ATIVOS: %d\n", nClientesAtivos);
                }
            }
        }
        else if (cmd[0] == 's')
        {
            printf("\n PAUSAR CLIENTE\n");
        }
        else if (cmd[0] == 'r')
        {
            printf("\nVOLTAR A JOGAR\n");
        }
        else if (strcmp(cmd, "end") == 0)
        {
            printf("\n TERMINAR\n");
            for (int i = 0; i < nClientesAtivos; i++)
            {

                strcpy(s[i].jogador.cmd, "end");
            }
        }
        else
        {
            printf("\nCOMANDO IMPOSSIVEL!\n");
        }

    } while (FLAG_SHUTDOWN != 1);

    end();
}