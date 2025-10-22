#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 2048

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";

    // Cria o socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar socket");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // Converte o endereço IP
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("Endereço inválido ou não suportado\n");
        return -1;
    }

    // Conecta ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erro ao conectar");
        return -1;
    }

    // Envia requisição HTTP
    send(sock, request, strlen(request), 0);
    printf("Requisição enviada:\n%s\n", request);

    // Recebe a resposta
    int bytes = read(sock, buffer, BUFFER_SIZE);
    if (bytes > 0) {
        printf("Resposta recebida:\n%s\n", buffer);
    }

    close(sock);
    return 0;
}
