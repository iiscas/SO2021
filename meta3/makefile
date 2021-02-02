all: cliente servidor jogo

cliente:
	gcc -c cliente.c
	gcc cliente.h cliente.c -o cliente 

servidor:
	gcc -c servidor.c 
	gcc servidor.h servidor.c -o servidor -lpthread 

jogo:
	gcc -c jogos/g_1.c
	gcc jogos/g_1.c -o  jogos/g_1
	gcc -c jogos/g_2.c
	gcc jogos/g_2.c -o  jogos/g_2

clean: cleanc cleans cleanj1 cleanj2

cleanc:
	rm cliente
	rm cliente.o

cleans:
	rm servidor
	rm servidor.o

cleanj1:
	rm jogos/g_1
	rm g_1.o
cleanj2:
	rm jogos/g_2
	rm g_2.o
