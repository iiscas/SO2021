#include "servidor.h"

typedef struct
{
    char letra;
    char continua;
    pthread_mutex_t *trinco;
} TDATA;
int findJogos()
{
    DIR *folder;
    struct dirent *entry;
    int files = 1;

    folder = opendir("./jogos");
    if (folder == NULL)
    {
        perror("Unable to read directory");
        //return(1);
    }

    while ((entry=readdir(folder))!=NULL)
    {
        
        if (entry->d_name[0] == 'g' && entry->d_name[strlen(entry->d_name) - 1] == 'c')
        {
            printf(" - %s\n", entry->d_name);
        }
        files++;
    }

    closedir(folder);
    return files;
    
}
char randomJogo()
{
    srand(time(NULL));
    DIR *jogo;
    struct dirent* dirJogos;
    int jogoAtual = 1;
    int jogoRandom;
    int nJogos = findJogos();

    //NUMERO RANDOM ENTRE 1 O NUMERO DE FILES LIDOS
    jogoRandom = rand() % nJogos;

    printf("Jogos disponiveis: %d\n", nJogos);
    printf("Numero random: %d\n", jogoRandom);

    jogo = opendir("./jogos");
    if (jogo)
    {
        while ((dirJogos = readdir(jogo)) != NULL)
        {
            if (dirJogos->d_name[0] == 'g' && dirJogos->d_name[strlen(dirJogos->d_name) - 1] == 'c')
            {

                printf("Jogo Atual: %d\n", jogoAtual);

                if (jogoRandom == jogoAtual)
                {

                    printf("Nome do jogo a devolver: %s\n", dirJogos->d_name);
                    dirJogos->d_name[strlen(dirJogos->d_name) - 2] = '\0';
                    return (*dirJogos->d_name);
                }

                jogoAtual++;
            }
        }
        closedir(jogo);
    }
}
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
void comecarCamp()
{
    printf("\n\n=======================================");
    printf("\n======= CAMPEONATO VAI COMECAR ========\n");
    printf("=======================================\n");
    FLAG_ADICIONA = 1;
}
void settings(int duracao, int tempo_espera)
{
    const char *maxplayer = getenv("MAXPLAYER");
    const char *gamedir = getenv("GAMEDIR");

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

int end()
{
    printf("A TERMINAR O SERVIDOR....ADEUS...\n");
    sleep(1);
    remove(FIFO_SERV);
    close(fd_ser);
    unlink(FIFO_SERV);
    return EXIT_SUCCESS;
}

typedef struct
{
    char jogo;
    char continua;
    pid_t pid[2];
    char resposta;
} TCAMPEONATO;

int p[2], r[2];
int filho;
void ClienteServidor(void* list){
    Cliente msgForCliente;
    Servidor msgForServidor;
    char fifo_name_cli[50];
    int tempo=0;
    //THREAD
    pthread_t campeonatoT;
    do{
        read(fd_ser,&msgForServidor,sizeof(msgForServidor));
    }
}
int main(int argc, char *argv[])
{
    Cliente c;
    Servidor s;
    s.nClientesAtivos = 0;
    int duracao, tempo_espera, i = 0, res, r, fds_anon, canal[2], res_canal, n, estado;
    char *ptr, *ptr1, *ptr2, fifo_name[20], cmd[90];

    nthread = 1;
    char str[40];
    int z, *resultado;
    TCAMPEONATO jogador[nthread];
    pthread_t tarefa[nthread];
    //pthread_mutex_t trinco;

    fd_set fds;
    struct timeval tempo;

    struct sigaction act;
    act.sa_handler = comecarCamp;
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);

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
    //findJogos();
    //randomJogo();
    //SELECT PARA LER DO TECLADO E COMANDOS DOS CLIENTES ( NA FASE DE CONFIGURAÇÕES APENAS?)
    do
    {

        fflush(stdout);
        printf("\nINTRODUZA UM COMANDO: ");
        fflush(stdout);

        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(fd_ser, &fds);

        tempo.tv_sec = 10; //vai buscar valor pela linha de comandos
        tempo.tv_usec = 0;
        res = select(fd_ser + 1, &fds, NULL, NULL, &tempo);

        if (res > 0 && FD_ISSET(0, &fds)) //receber do teclado
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
            else if (strcmp(cmd, "comeca") == 0)
            {
                FLAG_CAMPEONATO = 1;
            }
            else
            {
                printf("\nCOMANDO INVALIDO!\n");
            }
        }

        else if (res > 0 && FD_ISSET(fd_ser, &fds)) //receber do fifo
        {

            r = read(fd_ser, &c, sizeof(Cliente)); //abre o que o fd_ser enviou
            //printf("\nRECEBI DO FIFO %d ///%s %s %s \n", c.pid_cliente, c.nome, c.cmd, c.jogoAtribuido);
            if (r == sizeof(Cliente))
            {
                // COMANDOS PARA QUANDO JÁ HÁ CLIENTES
                if (existeCliente(c.nome, s) == 1)
                {
                    if (strcmp(c.cmd, "#mygame") == 0)
                    {

                        for (int i = 0; i < s.nClientesAtivos; i++)
                        {
                            if (c.pid_cliente == s.num_jogadores[i].pid_cliente)
                            {

                                strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);

                                //printf("aqui %s", c.jogoAtribuido);

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
                                printf("\n%s PRETENDE DESISTIR DO CAMPEONATO!\n", s.num_jogadores[i].nome);
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

                // PARA ADICIONAR NOVO CLIENTES DURANTE TEMPO DE ESPERA
                else
                {
                    if (FLAG_ADICIONA != 1)
                    {
                        alarm(tempo_espera);
                        randPorFuncao();
                        adicionaCLiente(c, &s);
                        fprintf(stderr, "\n%s INICIOU SESSAO\n", c.nome);

                        strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);
                        sprintf(fifo_name, "CLI%d", c.pid_cliente);
                        fd_cli = open(fifo_name, O_WRONLY);
                        r = write(fd_cli, &c, sizeof(Cliente));

                        close(fd_cli);
                    }

                    //PARA TERMINAR CLIENTE QUE TENTOU INICIAR DEPOIS DO TEMPO ACABAR
                    if (FLAG_ADICIONA == 1)
                    {
                        alarm(0); //confirmar necessidade de estar aqui
                        strcpy(c.jogoAtribuido, "quit");

                        sprintf(fifo_name, FIFO_CLI, c.pid_cliente);
                        fd_cli = open(fifo_name, O_WRONLY);
                        r = write(fd_cli, &c, sizeof(Cliente));

                        close(fd_cli);
                    }
                }
            }
        }
        nthread = s.nClientesAtivos;

    } while (/* FLAG_SHUTDOWN != 1 ||  */ FLAG_CAMPEONATO != 1);
}