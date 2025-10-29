#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define BUFFER_SIZE 4096

// Função para salvar os dados recebidos em um arquivo
void salvar_arquivo(const char *nome_arquivo, const char *dados, int tamanho) {
    FILE *fp = fopen(nome_arquivo, "ab"); // abre em modo append binário
    if (fp) {
        fwrite(dados, 1, tamanho, fp);
        fclose(fp);
    } else {
        perror("Erro ao salvar arquivo");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s http://host:porta/arquivo\n", argv[0]);
        return 1;
    }

    char host[256], caminho[256] = "", nome_arquivo[512];
    int porta = 80;

    // Analisa a URL: http://host[:porta]/caminho
    if (sscanf(argv[1], "http://%255[^:/]:%d/%255[^\n]", host, &porta, caminho) < 2) {
        porta = 80;
        sscanf(argv[1], "http://%255[^/]/%255[^\n]", host, caminho);
    }

    // Se caminho estiver vazio ou terminar com '/', usa "index.html"
    if (strlen(caminho) == 0 || caminho[strlen(caminho) - 1] == '/')
        strcat(caminho, "index.html");

    // Diretório de saída
    const char *diretorio_destino = "output";
    mkdir(diretorio_destino, 0777); // cria se não existir

    // Monta o nome do arquivo de saída
    char *nome = strrchr(caminho, '/');
    snprintf(nome_arquivo, sizeof(nome_arquivo), "%s/%s",
             diretorio_destino,
             (nome ? nome + 1 : caminho));

    // Remove apenas o arquivo se já existir
    if (access(nome_arquivo, F_OK) == 0) {
        remove(nome_arquivo);
    }

    printf("🔗 Conectando a %s:%d e requisitando /%s...\n", host, porta, caminho);

    // Resolve o host (DNS)
    struct hostent *servidor = gethostbyname(host);
    if (!servidor) {
        perror("Erro ao resolver host");
        return 2;
    }

    // Cria o socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erro ao criar socket");
        return 3;
    }

    // Configura o endereço do servidor
    struct sockaddr_in servidor_addr;
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_port = htons(porta);
    memcpy(&servidor_addr.sin_addr.s_addr, servidor->h_addr_list[0], servidor->h_length);

    // Conecta ao servidor
    if (connect(sock, (struct sockaddr*)&servidor_addr, sizeof(servidor_addr)) < 0) {
        perror("Erro ao conectar");
        close(sock);
        return 4;
    }

    // Monta e envia a requisição HTTP
    char requisicao[512];
    snprintf(requisicao, sizeof(requisicao),
             "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n", caminho, host);
    send(sock, requisicao, strlen(requisicao), 0);

    // Recebe a resposta
    char buffer[BUFFER_SIZE];
    int recebido;
    int cabecalho_terminou = 0;

    while ((recebido = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        if (!cabecalho_terminou) {
            char *p = strstr(buffer, "\r\n\r\n");
            if (p) {
                cabecalho_terminou = 1;
                int offset = p - buffer + 4;
                salvar_arquivo(nome_arquivo, buffer + offset, recebido - offset);
            }
        } else {
            salvar_arquivo(nome_arquivo, buffer, recebido);
        }
    }

    close(sock);
    printf("✅ Arquivo salvo em '%s'\n", nome_arquivo);

    return 0;
}
