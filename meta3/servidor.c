#include "servidor.h"
Cliente c;
Servidor s[30];
int res;
int nClientesAtivos = 0;
int i=0;
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

    while ((entry = readdir(folder)) != NULL)
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
    struct dirent *dirJogos;
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
void players(Servidor s[])
{
    if (nClientesAtivos == 0)
        printf("\nNAO HA JOGADORES LIGADOS!\n");
    else
    {
        printf("\n==== JOGADORES NO CAMPEONATO ====\n");
        for (int i = 0; i < nClientesAtivos; i++)
        {

            printf(" Jogador %s --- Jogo atribuido: %s \n", s[i].jogador.nome, s[i].cmd);
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
int existeCliente(char nome[], Servidor s[])
{
    int i;
    for (i = 0; i < nClientesAtivos; i++)
    {
        if (strcmp(s[i].jogador.nome, nome) == 0)
            return 1;
    }
    return 0;
}
int adicionaCLiente(Cliente a, Servidor *s[i])
{
    
    s[nClientesAtivos]->jogador = a;
    s[nClientesAtivos]->nJogos = rand() % 2;
    //        printf("\nNUMERO RANDOM: %d\n", s.nJogos);

    if (s[0]->nJogos == 0)
    {
        strcpy(a.cmd, "g_1");
        strcpy(s[nClientesAtivos]->jogador.cmd, "g_1.c");
    }
    else
    {
        strcpy(a.cmd, "g_2");
        strcpy(s[nClientesAtivos]->jogador.cmd, "g_2.c");
    }
    printf("Adicionei jogador %s ---- jogo: %s\n", s[nClientesAtivos]->jogador.nome, s[nClientesAtivos]->cmd);
    nClientesAtivos++;
    return 1;
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
void *ClienteServidor(void *dados)
{
    //fprintf(stderr, "\n Cliente com o PID %d esta a tentar conectar.\n", s.num_jogadores[s.nClientesAtivos].pid_cliente);
    Servidor *sClientes;
    sClientes = (Servidor *)dados;
    //msgForservidor=s
    char fifo_name[50];
    int r;
    char cmd[60];
    //s.nClientesAtivos = 0;
    //THREAD
    fd_set fds;

    pthread_t campeonatoT;
    struct timeval tempo;
    printf("aqui\n");
    
    
    r = read(fd_ser, &c, sizeof(Cliente)); //abre o que o fd_ser enviou
    fprintf(stderr, "\n Cliente com o PID %d esta a tentar conectar.\n", c.pid_cliente);
    printf("\nRECEBI DO FIFO %d -- %s----%s--- \n", c.pid_cliente, c.nome, c.cmd);

   /*  if (r == sizeof(Cliente))
    {

        //randPorFuncao();
        adicionaCLiente(c, &s[nClientesAtivos]);
        fprintf(stderr, "\n%s INICIOU SESSAO\n", c.nome);

        //strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);
        //printf("\n%s", c.jogoAtribuido);

        sprintf(fifo_name, "CLI%d", c.pid_cliente);
        //printf("NOME DO PIPE: %s\n", fifo_name);
        fd_cli = open(fifo_name, O_WRONLY);
        r = write(fd_cli, &c, sizeof(Cliente));

        close(fd_cli);
        //printf("ENVIEI ISTO %s", c.jogoAtribuido);
        //}
    }
    else
    {
        printf("ESTRUTURA RECEBIDA COM TAMANHO DIFERENTE!\n");
    } */
}
int main(int argc, char *argv[])
{
    //s.nClientesAtivos = 0;
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
    fd_ser = open(FIFO_SERV, O_RDWR );
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

    // -- Cria thread para a comunicação entre servidor e cliente  --
    pthread_t ligaCliServ;

    pthread_create(&ligaCliServ, NULL, ClienteServidor, (void *)&s);

    do
    {

        printf("\nINTRODUZA UM COMANDO: ");
        fgets(cmd, sizeof(cmd), stdin);
        strtok(cmd, "\n");

        if (strcmp(cmd, "players") == 0)
        {
            players(s);
        }
        else if (strcmp(cmd, "games") == 0)
        {
            games();
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            FLAG_SHUTDOWN = 1;
        }
        else if (cmd[0] == 'k')
        {
            char *token;
            token = strtok(cmd, "k");

            for (int i = 0; i < nClientesAtivos; i++)
            {
                if (strcmp(s[i].jogador.nome, token) == 0)
                    kill(s[i].jogador.pid_cliente, SIGINT);
            }
        }
        else if (cmd[0] == 's')
        {
            printf("\nStop Player\n");
        }
        else if (cmd[0] == 'r')
        {
            printf("\nResume Player\n");
        }
        else if (strcmp(cmd, "end") == 0)
        {
            printf("\nEND\n");
        }
        else
        {
            printf("\nComando nao detetado!\n");
        }
    } while (FLAG_SHUTDOWN != 1);

    /* r = read(fd_ser, &c, sizeof(Cliente)); //abre o que o fd_ser enviou
            printf("\nRECEBI DO FIFO %d ///%s %s %d \n", c.pid_cliente, c.nome, c.cmd, c.contador);

               
                adicionaCLiente(c, &s);
                fprintf(stderr, "\n%s INICIOU SESSAO\n", c.nome);

                // strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);
                //printf("\n%s", c.jogoAtribuido);

                sprintf(fifo_name, "CLI%d", c.pid_cliente);
                //printf("NOME DO PIPE: %s\n", fifo_name);
                fd_cli = open(fifo_name, O_WRONLY);
                r = write(fd_cli, &c, sizeof(Cliente));

                close(fd_cli);
                //printf("ENVIEI ISTO %s", c.jogoAtribuido); */

    printf("A TERMINAR O SERVIDOR....ADEUS...\n");
    sleep(1);

    remove(FIFO_SERV);
    close(fd_ser);
    unlink(FIFO_SERV);
    return EXIT_SUCCESS;
}