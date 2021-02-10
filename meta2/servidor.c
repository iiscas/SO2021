#include "servidor.h"

Servidor s;
Cliente c;
int fd_ser, fd_cli;

int FLAG_SHUTDOWN = 0;

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

char *recebeComando(char *cmd)
{
    char *str;
    if (strlen(cmd) == 0)
        str = cmd;
    else
    {
        str = cmd + 1;
        printf("\n%s\n", str);
    }
    return str;
}
void eliminaCliente(int pid)
{
    int i;
    for (i = 0; i < s.nClientesAtivos && s.num_jogadores[i].pid_cliente != pid; i++)
        ;
    if (i != s.nClientesAtivos)
    {
        printf("ADEUS %s ....\n", s.num_jogadores[i].nome);
        s.num_jogadores[i] = s.num_jogadores[s.nClientesAtivos - 1];
        s.nClientesAtivos--;
        printf("\nCLIENTE ELIMINADO: %s ", s.num_jogadores[i].nome);
        printf("\nNR DE CLIENTES AINDA ATIVOS: %d\n", s.nClientesAtivos);
    }
}
int existeCliente(char nome[])
{
    int i;
    for (i = 0; i < s.nClientesAtivos; i++)
    {
        if (strcmp(s.num_jogadores[i].nome, nome) == 0)
            return 1;
    }
    return 0;
}
int adicionaCLiente(Cliente a)
{
    if (existeCliente(a.nome))
    {
        return 0;
    }
    else
    {
        s.num_jogadores[s.nClientesAtivos] = a;
        s.nJogos = rand() % 2;
        //        printf("\nNUMERO RANDOM: %d\n", s.nJogos);

        if (s.nJogos == 0)
        {
            strcpy(a.jogoAtribuido, "g_1");
            strcpy(s.num_jogadores[s.nClientesAtivos].jogoAtribuido, "g_1");
        }
        else
        {
            strcpy(a.jogoAtribuido, "g_2");
            strcpy(s.num_jogadores[s.nClientesAtivos].jogoAtribuido, "g_2");
        }
        //printf("Adicionei jogador %s --  comandos: %s -- jogo: %s\n", s.num_jogadores[s.nClientesAtivos].nome, s.cmd, s.num_jogadores[s.nClientesAtivos].jogoAtribuido);
        s.nClientesAtivos++;
        return 1;
    }
}

/* void terminaServidor(int i)
{
    fprintf(stderr, "\n \nSERVIDOR DO CAMPEONATO A TERMINAR...", "VIA TECLADO \n\n");
    close(fd_ser);
    unlink(FIFO_SERV);
    exit(EXIT_SUCCESS);
} */

void players()
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
    printf("\nHa 2 jogos disponiveis:\n");
    printf("-- g_1\n");
    printf("-- g_2\n");
}
void settings()
{
    printf("\n=======================================");
    printf("\n======= BEM VINDO AO CHAMPION =========\n");
    printf("=======================================\n");

    printf("\n------- Configuracoes Iniciais --------\n");
    printf("\no Duracao do campeonato: %d s\n", duracao);
    printf("o Valor de tempo de espera: %d s\n", tempo_espera);
    printf("o Numero maximo de jogadores: %d\n", maxplayer);
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

int main(int argc, char *argv[])
{

    int i = 0, res, r;
    s.nClientesAtivos = 0;
    char *ptr, *ptr1, *ptr2, fifo_name[20], cmd[90];
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

    settings();
    comandosMenu();

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
                players();
            }

            else if (cmd[0] == 'k')
            {
                // printf("\nK\n");
                for (int i = 0; i < s.nClientesAtivos; i++)
                {
                    if (strcmp(recebeComando(cmd), s.num_jogadores[i].nome) == 0)
                        eliminaCliente(s.num_jogadores[i].pid_cliente);
                    strcpy(c.jogoAtribuido, "quit");

                    eliminaCliente(s.num_jogadores[i].pid_cliente);
                    sprintf(fifo_name, FIFO_CLI, c.pid_cliente);
                    //printf("NOME DO PIPE: %s\n", fifo_name);
                    fd_cli = open(fifo_name, O_WRONLY);
                    r = write(fd_cli, &c, sizeof(Cliente));

                    close(fd_cli);
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
                if (existeCliente(c.nome) == 1)
                {
                    if (strcmp(c.cmd, "#mygame") == 0)
                    {

                        for (int i = 0; i < s.nClientesAtivos; i++)
                        {
                            if (c.pid_cliente == s.num_jogadores[i].pid_cliente)
                            {

                                strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);

                                //printf("%s", c.jogoAtribuido);

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

                                eliminaCliente(s.num_jogadores[i].pid_cliente);
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
                else
                {
                    randPorFuncao();
                    adicionaCLiente(c);
                    fprintf(stderr, "\n%s INICIOU SESSAO\n", c.nome);

                    strcpy(c.jogoAtribuido, s.num_jogadores[i].jogoAtribuido);
                    //printf("\n%s", c.jogoAtribuido);

                    sprintf(fifo_name, "CLI%d", c.pid_cliente);
                    //printf("NOME DO PIPE: %s\n", fifo_name);
                    fd_cli = open(fifo_name, O_WRONLY);
                    r = write(fd_cli, &c, sizeof(Cliente));

                    close(fd_cli);
                    //printf("ENVIEI ISTO %s", c.jogoAtribuido);
                }
                //}
            }
            else
            {
                printf("ESTRUTURA RECEBIDA COM TAMANHO DIFERENTE!\n");
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