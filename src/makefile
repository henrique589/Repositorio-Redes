CC=gcc
CFLAGS=-Wall

all: cliente servidor

cliente: cliente_http.c
	$(CC) $(CFLAGS) cliente_http.c -o meu_navegador

servidor: servidor_http.c
	$(CC) $(CFLAGS) servidor_http.c -o meu_servidor

clean:
	rm -f meu_navegador meu_servidor
