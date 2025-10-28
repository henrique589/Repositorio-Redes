#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 4096

void usage(const char *prog) {
    printf("Uso: %s http://host/arquivo\n", prog);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) usage(argv[0]);

    char host[256], path[1024];
    if (sscanf(argv[1], "http://%255[^/]/%1023[^\n]", host, path) < 2) {
        fprintf(stderr, "URL inválida.\n");
        return 1;
    }

    // Resolve hostname
    struct hostent *server = gethostbyname(host);
    if (!server) {
        fprintf(stderr, "Erro: host não encontrado.\n");
        return 1;
    }

    // Cria socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    // Envia requisição HTTP
    char request[2048];
    snprintf(request, sizeof(request),
             "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
    send(sock, request, strlen(request), 0);

    // Nome do arquivo de saída
    char *filename = strrchr(path, '/');
    filename = filename ? filename + 1 : path;
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        close(sock);
        return 1;
    }

    // Lê resposta e ignora cabeçalhos HTTP
    char buffer[BUFFER_SIZE];
    int header_end = 0;
    while (1) {
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;

        if (!header_end) {
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                header_end = 1;
                body += 4;
                fwrite(body, 1, bytes - (body - buffer), fp);
            }
        } else {
            fwrite(buffer, 1, bytes, fp);
        }
    }

    fclose(fp);
    close(sock);
    printf("Arquivo '%s' salvo com sucesso!\n", filename);
    return 0;
}
