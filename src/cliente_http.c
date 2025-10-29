#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

void criar_diretorio(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s http://host:porta/arquivo\n", argv[0]);
        return 1;
    }

    char url[512];
    strncpy(url, argv[1], sizeof(url));
    url[sizeof(url) - 1] = '\0';

    // Verifica se começa com http://
    if (strncmp(url, "http://", 7) != 0) {
        fprintf(stderr, "URL inválida. Use o formato http://host[:porta]/arquivo\n");
        return 1;
    }

    char *host = url + 7;
    char *path = strchr(host, '/');
    char *port_str = strchr(host, ':');
    int port = 80;

    if (port_str && (!path || port_str < path)) {
        *port_str = '\0';
        port = atoi(port_str + 1);
    }

    if (path) {
        *path = '\0';
        path++;
    } else {
        path = "";
    }

    // Resolve hostname
    struct hostent *server = gethostbyname(host);
    if (!server) {
        fprintf(stderr, "Erro: host não encontrado: %s\n", host);
        return 1;
    }

    // Cria socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(port);

    // Conecta
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erro ao conectar");
        close(sockfd);
        return 1;
    }

    // Monta requisição GET
    char request[1024];
    snprintf(request, sizeof(request),
             "GET /%s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
             path, host);

    // Envia
    send(sockfd, request, strlen(request), 0);

    // Cria diretório de saída
    criar_diretorio("output");

    // Define nome do arquivo
    const char *filename = strrchr(path, '/');
    if (!filename) filename = path;
    else filename++;

    if (strlen(filename) == 0)
        filename = "index.html";

    char output_path[512];
    snprintf(output_path, sizeof(output_path), "output/%s", filename);

    FILE *outfile = fopen(output_path, "wb");
    if (!outfile) {
        perror("Erro ao criar arquivo de saída");
        close(sockfd);
        return 1;
    }

    // Recebe e salva
    char buffer[BUFFER_SIZE];
    int header_done = 0;
    while (1) {
        ssize_t bytes = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes <= 0)
            break;

        // Remove cabeçalhos HTTP
        if (!header_done) {
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                header_done = 1;
                body += 4;
                fwrite(body, 1, bytes - (body - buffer), outfile);
            }
        } else {
            fwrite(buffer, 1, bytes, outfile);
        }
    }

    printf("Arquivo salvo em %s\n", output_path);

    fclose(outfile);
    close(sockfd);
    return 0;
}
