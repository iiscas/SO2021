#include "servidor.h"
Cliente c;
Servidor s;
int res;
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
void players(Servidor s)
{
    if (s.nClientesAtivos == 0)
        printf("\nNAO HA JOGADORES LIGADOS!\n");
    else
    {
        printf("\n==== JOGADORES NO CAMPEONATO ====\n");
        for (int i = 0; i < s.nClientesAtivos; i++)
        {

            printf(" Jogador %s --- Jogo atribuido: %s \n", s.num_jogadores[i].nome, s.num_jogadores[i].cmd);
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
            strcpy(a.cmd, "g_1");
            strcpy(s->num_jogadores[s->nClientesAtivos].cmd, "g_1.c");
        }
        else
        {
            strcpy(a.cmd, "g_2");
            strcpy(s->num_jogadores[s->nClientesAtivos].cmd, "g_2.c");
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
void* ClienteServidor(void *dados)
{
    fprintf(stderr, "\n Cliente com o PID %d esta a tentar conectar.\n", s.num_jogadores[s.nClientesAtivos].pid_cliente);
    Servidor *sClientes;
    sClientes=(Servidor*)dados;
    //msgForservidor=s
    char fifo_name_cli[50];
    int tempo = 0;
    s.nClientesAtivos = 0;
    //THREAD
    pthread_t campeonatoT;
    do
    {
        read(fd_ser, &s, sizeof(s));

        if (s.nClientesAtivos == 0)
            fprintf(stderr, "\n Cliente com o PID %d esta a tentar conectar.\n", s.num_jogadores[s.nClientesAtivos].pid_cliente);
        else
            fprintf(stderr, "\n Mensagem recebida do cliente com o PID %d: [%s]\n", s.num_jogadores[s.nClientesAtivos].pid_cliente, s.num_jogadores[s.nClientesAtivos].cmd);

        sprintf(fifo_name_cli, FIFO_CLI, s.num_jogadores[s.nClientesAtivos].pid_cliente);

        if (s.nClientesAtivos < 30)
        {
            adicionaCLiente(c, &s);
            fprintf(stderr, "\n%s INICIOU SESSAO\n", c.nome);
            strcpy(c.cmd,"JA PODE INSERIR COMANDOS!\n\n");
            strcpy(c.cmd, s.num_jogadores[s.nClientesAtivos].cmd);
            //sprintf(fifo_name, "CLI%d", c.pid_cliente);
            fd_cli=open(fifo_name_cli, O_WRONLY);
            res = write(fd_cli, &c, sizeof(Cliente));

            /* if(s.nClientesAtivos>=2 && tempo==0){
                tempo==1;
                pthread_create()
            } */

            close(fd_cli);
        }
    }while(1);
}
int main(int argc, char *argv[])
{
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
    s.num_jogadores[1];
    printf("aqui");
    Servidor serv[1];
    
    // -- Cria thread para a comunicação entre servidor e cliente  --
	pthread_t mainThread;
     printf("aqui");
	pthread_create(&mainThread, NULL, ClienteServidor, (void*)&s);
    printf("aqui");
    sleep(2);
}