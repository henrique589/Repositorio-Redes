#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define PORTA 5050
#define BUFFER_SIZE 4096

void enviar_arquivo(int cliente, const char *caminho) {
    FILE *file = fopen(caminho, "rb");
    if (!file) {
        const char *msg = "HTTP/1.0 404 Not Found\r\n\r\nArquivo não encontrado.";
        send(cliente, msg, strlen(msg), 0);
        return;
    }

    const char *header = "HTTP/1.0 200 OK\r\n\r\n";
    send(cliente, header, strlen(header), 0);

    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(cliente, buffer, bytes, 0);
    }

    fclose(file);
}

void listar_diretorio(int cliente, const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        const char *msg = "HTTP/1.0 500 Internal Server Error\r\n\r\nErro ao abrir diretório.";
        send(cliente, msg, strlen(msg), 0);
        return;
    }

    const char *header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
    send(cliente, header, strlen(header), 0);
    send(cliente, "<html><body><h2>Arquivos disponiveis:</h2><ul>", 56, 0);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        char item[512];
        snprintf(item, sizeof(item), "<li><a href=\"/%s\">%s</a></li>", entry->d_name, entry->d_name);
        send(cliente, item, strlen(item), 0);
    }

    send(cliente, "</ul></body></html>", 20, 0);
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <diretorio>\n", argv[0]);
        return 1;
    }

    const char *diretorio = argv[1];
    int servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    struct sockaddr_in endereco;
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA);

    int opt = 1;
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(servidor, (struct sockaddr *)&endereco, sizeof(endereco)) < 0) {
        perror("Erro no bind");
        close(servidor);
        return 1;
    }

    listen(servidor, 10);
    printf("Servidor HTTP iniciado na porta %d. Servindo diretório: %s\n", PORTA, diretorio);

    while (1) {
        int cliente = accept(servidor, NULL, NULL);
        if (cliente < 0) continue;

        char buffer[BUFFER_SIZE];
        recv(cliente, buffer, sizeof(buffer) - 1, 0);

        char metodo[8], caminho[256];
        sscanf(buffer, "%s %s", metodo, caminho);

        if (strcmp(metodo, "GET") != 0) {
            const char *msg = "HTTP/1.0 405 Method Not Allowed\r\n\r\n";
            send(cliente, msg, strlen(msg), 0);
            close(cliente);
            continue;
        }

        if (strcmp(caminho, "/") == 0) {
            char index_path[512];
            snprintf(index_path, sizeof(index_path), "%s/index.html", diretorio);
            struct stat st;
            if (stat(index_path, &st) == 0)
                enviar_arquivo(cliente, index_path);
            else
                listar_diretorio(cliente, diretorio);
        } else {
            char arquivo_path[512];
            snprintf(arquivo_path, sizeof(arquivo_path), "%s%s", diretorio, caminho);
            enviar_arquivo(cliente, arquivo_path);
        }

        close(cliente);
    }

    close(servidor);
    return 0;
}
