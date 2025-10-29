#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define PORTA 8080
#define BUFFER_SIZE 4096

void enviar_arquivo(int cliente, const char *caminho) {
    FILE *fp = fopen(caminho, "rb");
    if (!fp) {
        char erro[] = "HTTP/1.0 404 Not Found\r\nContent-Type: text/plain\r\n\r\nArquivo não encontrado.\n";
        send(cliente, erro, strlen(erro), 0);
        return;
    }

    // Cabeçalho simples
    char cabecalho[] = "HTTP/1.0 200 OK\r\n\r\n";
    send(cliente, cabecalho, strlen(cabecalho), 0);

    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        send(cliente, buffer, bytes, 0);

    fclose(fp);
}

// Lista os arquivos de um diretório em HTML
void listar_diretorio(int cliente, const char *diretorio, const char *caminho_url) {
    DIR *dir = opendir(diretorio);
    if (!dir) {
        char erro[] = "HTTP/1.0 500 Internal Server Error\r\n\r\nErro ao abrir diretório.";
        send(cliente, erro, strlen(erro), 0);
        return;
    }

    char cabecalho[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
    send(cliente, cabecalho, strlen(cabecalho), 0);

    char html[BUFFER_SIZE];
    snprintf(html, sizeof(html),
             "<html><head><meta charset='utf-8'>"
             "<title>Listagem de %s</title>"
             "<style>"
             "body{font-family:Arial;background:#f9f9f9;padding:20px;}"
             "ul{list-style:none;padding:0;}"
             "li{margin:5px 0;}"
             "a{text-decoration:none;color:#007bff;}"
             "a:hover{text-decoration:underline;}"
             "</style>"
             "</head><body><h2>📂 Conteúdo de %s</h2><ul>",
             diretorio, diretorio);
    send(cliente, html, strlen(html), 0);

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char link[512];
        if (strcmp(caminho_url, "/") == 0)
            snprintf(link, sizeof(link), "/%s", ent->d_name);
        else
            snprintf(link, sizeof(link), "%s/%s", caminho_url, ent->d_name);

        snprintf(html, sizeof(html), "<li><a href=\"%s\">%s</a></li>", link, ent->d_name);
        send(cliente, html, strlen(html), 0);
    }

    strcpy(html, "</ul><hr><p><em>Servidor HTTP em C - Porta 5050</em></p></body></html>");
    send(cliente, html, strlen(html), 0);

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s /caminho/para/pasta\n", argv[0]);
        return 1;
    }

    const char *diretorio_base = argv[1];

    int servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTA);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(servidor, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Erro no bind");
        close(servidor);
        return 1;
    }

    listen(servidor, 10);
    printf("🚀 Servidor rodando na porta %d, servindo '%s'\n", PORTA, diretorio_base);

    while (1) {
        int cliente = accept(servidor, NULL, NULL);
        if (cliente < 0) continue;

        char buffer[BUFFER_SIZE] = {0};
        recv(cliente, buffer, sizeof(buffer), 0);

        char metodo[8], caminho[256];
        sscanf(buffer, "%s %s", metodo, caminho);

        if (strcmp(metodo, "GET") != 0) {
            close(cliente);
            continue;
        }

        // 🔧 Monta caminho absoluto corretamente
        char caminho_completo[512];
        if (diretorio_base[strlen(diretorio_base) - 1] == '/')
            snprintf(caminho_completo, sizeof(caminho_completo), "%s%s", diretorio_base, caminho);
        else
            snprintf(caminho_completo, sizeof(caminho_completo), "%s%s", diretorio_base,
                     (strcmp(caminho, "/") == 0) ? "" : caminho);

        printf("📂 Caminho completo solicitado: %s\n", caminho_completo);

        struct stat st;
        if (stat(caminho_completo, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                char index_path[512];
                snprintf(index_path, sizeof(index_path), "%s/index.html", caminho_completo);
                if (access(index_path, F_OK) == 0)
                    enviar_arquivo(cliente, index_path);
                else
                    listar_diretorio(cliente, caminho_completo, caminho);
            } else {
                enviar_arquivo(cliente, caminho_completo);
            }
        } else {
            char erro[] = "HTTP/1.0 404 Not Found\r\n\r\nArquivo não encontrado.";
            send(cliente, erro, strlen(erro), 0);
        }

        close(cliente);
    }

    close(servidor);
    return 0;
}
