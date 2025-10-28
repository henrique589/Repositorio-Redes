#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096
#define PORT 8080

void send_response(int client, const char *path) {
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "./imagens/%s", path[0] == '/' ? path + 1 : path);

    if (strcmp(path, "/") == 0)
        strcpy(fullpath, "./index.html");  // caso acesse sem arquivo específico

    FILE *fp = fopen(fullpath, "rb");
    if (!fp) {
        char *notfound = "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 - Arquivo não encontrado</h1>";
        send(client, notfound, strlen(notfound), 0);
        return;
    }

    // Determina o tipo MIME básico
    char *ext = strrchr(fullpath, '.');
    char content_type[64] = "application/octet-stream";
    if (ext) {
        if (strcmp(ext, ".html") == 0) strcpy(content_type, "text/html");
        else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) strcpy(content_type, "image/jpeg");
        else if (strcmp(ext, ".png") == 0) strcpy(content_type, "image/png");
        else if (strcmp(ext, ".gif") == 0) strcpy(content_type, "image/gif");
    }

    // Envia cabeçalhos HTTP
    char header[256];
    snprintf(header, sizeof(header),
             "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
    send(client, header, strlen(header), 0);

    // Envia o conteúdo do arquivo
    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(client, buffer, bytes, 0);
    }

    fclose(fp);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    listen(server_fd, 5);
    printf("Servidor rodando em http://localhost:%d/foto.jpg\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client < 0) continue;

        char buffer[BUFFER_SIZE];
        int bytes = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            char method[8], path[256];
            sscanf(buffer, "%s %s", method, path);
            if (strcmp(method, "GET") == 0) {
                send_response(client, path);
            }
        }
        close(client);
    }

    close(server_fd);
    return 0;
}
