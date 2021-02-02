#include "cliente.h"
int FLAG_SHUTDOWN = 0;
int fd_ser, fd_cli;
char fifo_name[20];
//Servidor s;

void menu_comandos()
{
    printf("\n=======================================");
    printf("\n======= BEM VINDO AO CHAMPION =========\n");
    printf("=======================================\n");

    printf("\n----------COMANDOS DISPONIVEIS--------\n");
    printf("\no Consultar jogo <#mygame>\n");
    printf("\no Sair do campeonato <#quit>\n");
}

void sair()
{
    FLAG_SHUTDOWN = 1;
    printf("\nCLIENTE VAI DESLIGAR\n");
    sleep(1);
    //close(fd_ser);
    close(fd_cli);
    unlink(fifo_name);
    //unlink(FIFO_CLI);
    //unlink(FIFO_SERV);
    exit(EXIT_SUCCESS);
}

void trataSig(int i)
{
    fprintf(stderr, "\nCLIENTE A TERMINAR VIA TECLADO\n");
    close(fd_ser);
    close(fd_cli);
    unlink(fifo_name);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int res, i = 0;
    char cmd[60];
    fd_set fds;
    Cliente c,cr;
    struct timeval tempo;

    if (signal(SIGINT, trataSig) == SIG_ERR)
    {
        perror("\nNAO FOI POSSIVEL CONFIGURAR SINAL SIGNINT!\n");
        exit(EXIT_FAILURE);
    }
    if (access(FIFO_SERV, F_OK) != 0)
    {
        fprintf(stderr, "\n[ERRO] NAO HA NENHUM SERVIDOR!\n");
        exit(EXIT_FAILURE);
    }
   

    menu_comandos();

    c.pid_cliente = getpid();
    sprintf(fifo_name, FIFO_CLI, c.pid_cliente);
     if (access(fifo_name, F_OK) == 0)
    {
        fprintf(stderr, "\n[ERRO] CLIENTE JA EXISTE!\n");
        exit(EXIT_FAILURE);
    }
    mkfifo(fifo_name, 0600);
    fd_cli = open(fifo_name, O_RDWR);
    fprintf(stderr, "\n CRIOU O FIFO %s!\n", fifo_name);

    strcpy(c.cmd, "");
    strcpy(c.jogoAtribuido, "");

    printf("\nIntroduza o seu nome: ");
    scanf("%s", c.nome);
    fflush(stdout);

    //ABRE O FIFO DO SERV PARA ENVIO DO NOME
    fd_ser = open(FIFO_SERV, O_WRONLY);
    printf("\nABRI O FIFO DO SERVIDOR %s", FIFO_SERV);

    res = write(fd_ser, &c, sizeof(Cliente));
    printf("\nENVIEI %s ", c.nome);
    close(fd_ser);

    do
    {
        printf("\n%s, INTRODUZA UM COMANDO: ", c.nome);

        fflush(stdout);
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(fd_cli, &fds);

        tempo.tv_sec = 20; //vai buscar valor pela linha de comandos
        tempo.tv_usec = 0;

        res = select(fd_cli + 1, &fds, NULL, NULL, &tempo);

        if (res == 0)
        {
            printf("NAO HA DADOS \n");
        }

        if (res > 0 && FD_ISSET(0, &fds))
        {
            scanf("%s", c.cmd);
            //printf("TESTE SCANF %s\n", c.cmd);

            //ABRE O FIFO DO SERV PARA ENVIO DO NOME
            fd_ser = open(FIFO_SERV, O_WRONLY);
            printf("\nABRI O FIFO DO CLIENTE %s", FIFO_SERV);

            res = write(fd_ser, &c, sizeof(Cliente));
            //printf("\nENVIEI %s %s %s %d\n", c.nome, c.cmd ,c.jogoAtribuido,c.pid_cliente);
            close(fd_ser);
        }

        else if (res > 0 && FD_ISSET(fd_cli, &fds))
        {
            //RECEBE RESPOSTA DO SERVIDOR
            res = read(fd_cli, &cr, sizeof(Cliente));

            if (strcmp(cr.jogoAtribuido, "quit") == 0)
            {
                printf("%d, comando: ");
                sair();
            }
            else
            {
                printf("\n%s, tem este jogo atribuido: %s\n", cr.nome, cr.jogoAtribuido);
            }
        }

    } while (FLAG_SHUTDOWN != 1);

    close(fd_cli);
    unlink(fifo_name);
    unlink(FIFO_SERV);
    exit(0);
}