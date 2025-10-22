#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Resposta HTTP simples
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 56\r\n"
        "\r\n"
        "<html><body><h1>Servidor HTTP em C!</h1></body></html>";

    // Criação do socket TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    // Configurações do endereço
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associação do socket à porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erro no bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Coloca o servidor em modo de escuta
    if (listen(server_fd, 3) < 0) {
        perror("Erro no listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor HTTP rodando na porta %d...\n", PORT);

    // Aceita uma conexão
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
        perror("Erro ao aceitar conexão");
        exit(EXIT_FAILURE);
    }

    // Lê a requisição do cliente
    read(new_socket, buffer, BUFFER_SIZE);
    printf("Requisição recebida:\n%s\n", buffer);

    // Envia a resposta HTTP
    send(new_socket, response, strlen(response), 0);
    printf("Resposta enviada ao cliente.\n");

    close(new_socket);
    close(server_fd);
    return 0;
}
