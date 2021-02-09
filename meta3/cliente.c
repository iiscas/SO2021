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
    close(fd_ser);
    close(fd_cli);
    unlink(fifo_name);
    unlink(FIFO_CLI);
    unlink(FIFO_SERV);
    exit(EXIT_SUCCESS);
}

void handlerSIG(int sig)
{
    if (sig == SIGINT)
    {
        fprintf(stderr, "\nCLIENTE A DESLIGAR.....\n");
        FLAG_SHUTDOWN = 1;
        sleep(1);
        close(fd_ser);
        close(fd_cli);
        unlink(fifo_name);
        exit(EXIT_SUCCESS);
    }
}
int main(int argc, char *argv[])
{
    int res, i = 0, r;
    char cmd[60];
    fd_set fds;
    Cliente c, cr;
    struct timeval tempo;

    if (signal(SIGINT, handlerSIG) == SIG_ERR)
    {
        perror("\n NAO FOI POSSIVEL SINAL SIGNINT\n");
        exit(EXIT_FAILURE);
    }

    if (access(FIFO_SERV, F_OK) != 0)
    {
        fprintf(stderr, "\n[ERRO] NAO HA NENHUM SERVIDOR!\n");
        exit(EXIT_FAILURE);
    }

    menu_comandos();
    strcpy(c.cmd, "");
    c.pid_cliente = getpid(); //identifica o cliente
    sprintf(fifo_name, FIFO_CLI, c.pid_cliente);

    if (mkfifo(fifo_name, 0600) < 0)
    {
        perror("\n Erro ao criar o FIFO do cliente.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO %s do cliente criado.\n", fifo_name);

    fd_ser = open(FIFO_SERV, O_WRONLY); //abre o FIFO do servidor para escrita
    if (fd_ser < 0)
    {
        fprintf(stderr, "\n O cliente não consegue ligar-se ao servidor.\n");
        unlink(fifo_name);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do servidor aberto para escrita.\n");

    if ((fd_cli = open(fifo_name, O_RDWR)) < 0)
    { //o fifo do cliente so le
        perror("\n Erro ao abrir o FIFO do cliente.\n");
        close(fd_ser);
        unlink(fifo_name);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do Cliente aberto para leitura.\n");

    fd_cli = open(fifo_name, O_RDWR);

    printf("\nINTRODUZA O SEU NOME: ");
    scanf("%s", c.nome);
    fflush(stdout);
    c.acesso = 0;
    //ABRE O FIFO DO SERV PARA ENVIO DO NOME
    fd_ser = open(FIFO_SERV, O_WRONLY);
    write(fd_ser, &c, sizeof(Cliente));

    res = read(fd_cli, &c, sizeof(Cliente));
    if (res == sizeof(Cliente))
    {
        fprintf(stderr, c.cmd);
    }
    else
    {
        fprintf(stderr, "\nNAO ENVIOU AO SERVIDOR !\n");
    }
    do
    {

        fflush(stdout);
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(fd_cli, &fds);

        tempo.tv_sec = 20; //vai buscar valor pela linha de comandos
        tempo.tv_usec = 0;

        r = select(fd_cli + 1, &fds, NULL, NULL, &tempo);

        if (r > 0 && FD_ISSET(0, &fds))
        {
            //stdin
            scanf("%s", c.cmd);

            c.acesso = 1;
            if (strcmp(c.cmd, "fim") == 0)
            {
                break;
            }

            res = write(fd_ser, &c, sizeof(Cliente));
        }

        else if (res > 0 && FD_ISSET(fd_cli, &fds))
        {
            //RECEBE RESPOSTA DO SERVIDOR
            res = read(fd_cli, &cr, sizeof(Cliente));

            if (strcmp(cr.cmd, "#quit") == 0)
            {
                sair();
            }
            else if (strcmp(cr.cmd, "#mygame") == 0)
            {
                printf("\n%s, TEM ESTE JOGO ATRIBUIDO: %s\n", cr.nome, cr.jogo);
            }
            // para imprimir jogo
            else
            {
                printf("\n%s", cr.cmd);
            }
        }

    } while (FLAG_SHUTDOWN != 1);
    close(fd_cli);
    close(fd_ser);
    unlink(fifo_name);
    unlink(FIFO_SERV);
    remove(fifo_name);

    exit(0);
}