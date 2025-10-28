# 🧩 Projeto: Cliente e Servidor Web em C
# Autor: Henrique Azevedo Andrade Silva
# Disciplina: Redes de Computadores

# ========================
# 🔹 Nomes dos executáveis
# ========================
CLIENTE = meunavegador
SERVIDOR = meuservidor

# ========================
# 🔹 Arquivos fonte
# ========================
CLIENTE_SRC = cliente_http.c
SERVIDOR_SRC = servidor_http.c

# ========================
# 🔹 Compilador e flags
# ========================
CC = gcc
CFLAGS = -Wall

# ========================
# 🔹 Bibliotecas (Windows/Linux)
# ========================
ifeq ($(OS),Windows_NT)
    LIBS = -lws2_32
else
    LIBS =
endif

# ========================
# 🏗️  Regras de compilação
# ========================
all: $(CLIENTE) $(SERVIDOR)

$(CLIENTE): $(CLIENTE_SRC)
	@echo "🔧 Compilando cliente HTTP..."
	$(CC) $(CFLAGS) $(CLIENTE_SRC) -o $(CLIENTE) $(LIBS)
	@echo "✅ Cliente compilado -> ./$(CLIENTE)"

$(SERVIDOR): $(SERVIDOR_SRC)
	@echo "🔧 Compilando servidor HTTP..."
	$(CC) $(CFLAGS) $(SERVIDOR_SRC) -o $(SERVIDOR) $(LIBS)
	@echo "✅ Servidor compilado -> ./$(SERVIDOR)"

# ========================
# 🚀 Execução rápida
# ========================
run-server:
	@echo "🌐 Iniciando servidor na porta 8080..."
	@./$(SERVIDOR)

run-client:
	@echo "💻 Executando cliente para baixar arquivo..."
	@./$(CLIENTE) http://localhost/foto.jpg

# ========================
# 🧹 Limpeza
# ========================
clean:
	@echo "🧹 Limpando binários..."
	rm -f $(CLIENTE) $(SERVIDOR)
