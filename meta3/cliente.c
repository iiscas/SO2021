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
    //fd_set fds;
    Cliente c, cr;
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

    /* if ((fd_cli = open(fifo_name, O_RDWR)) < 0)
    { //o fifo do cliente so le
        perror("\n Erro ao abrir o FIFO do cliente.\n");
        close(fd_ser);
        unlink(fifo_name);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\n FIFO do Cliente aberto para leitura.\n"); */

    //A este ponto o cliente pode escrever no fifo do servidor, que este vai poder ler
    //E pode ler o fifo do cliente, no qual o servidor vai estar a escrever

    printf("Nome de utilizador: ");
    scanf("%s", c.nome);
    fflush(stdout);
    strcpy(c.cmd, c.nome);
    printf("Nome de utilizador---> : %s", c.nome);
    res = write(fd_ser, &c, sizeof(Cliente));
    printf("\nENVIEI %s %d %s", c.nome,c.pid_cliente,c.cmd);
    close(fd_ser);
    sleep(10);
    /* fd_ser = open(FIFO_SERV, O_WRONLY);                      // Indica ao árbitro que é a primeira mensagem que envia.
    write(fd_ser, &c, sizeof(Cliente));      //escreve no fifo do servidor para ele ler usando a mensagem para servidor
    res = read(fd_cli, &c, sizeof(Cliente)); */  //le a mensagem do server no pipe do cliente
   /*  if (res == sizeof(Cliente))
    {
        fprintf(stderr, "\nO cliente recebeu mensagem do servidor.\n");
        fprintf(stderr, c.cmd);
    }
    else
    {
        fprintf(stderr, "\nO servidor não conseguiu entregar a mensagem ao cliente.\n");
    } */

   /*  while (1)
    {
        scanf("%s", c.cmd); //input do user
        c.contador = 1;     // A partir daqui todas as mensagens são comandos.
        if (strcmp(c.cmd, "fim") == 0)
        {
            break; //se fim vai fechar pipes e sair
        }
        write(fd_ser, &c, sizeof(Cliente)); //escreve no fifo do servidor para ele ler usando a mensagem para servidor

        res = read(fd_cli, &c, sizeof(Cliente)); //le a mensagem do server no pipe do cliente
        if (res == sizeof(Cliente))
        {
            fprintf(stderr, "\nO cliente recebeu mensagem do servidor.\n");
            fprintf(stderr, c.cmd);
        }
        else
        {
            fprintf(stderr, "\nO servidor não conseguiu entregar a mensagem ao cliente.\n");
        }
    } */

    /* close(fd_cli);
    unlink(fifo_name);
    unlink(FIFO_SERV);
    remove(fifo_name);

    exit(0); */
}