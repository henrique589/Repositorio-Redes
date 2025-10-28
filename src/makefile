# 🧩 Projeto: Cliente e Servidor Web em C
# Autor: Henrique Azevedo Andrade Silva
# Disciplina: Redes de Computadores

# Nome dos executáveis
CLIENTE = meunavegador
SERVIDOR = meuservidor

CLIENTE_SRC = cliente_http.c
SERVIDOR_SRC = servidor_http.c

CC = gcc

CFLAGS = -Wall

all: $(CLIENTE) $(SERVIDOR)

$(CLIENTE): $(CLIENTE_SRC)
	@echo "🔧 Compilando cliente HTTP..."
	$(CC) $(CFLAGS) $(CLIENTE_SRC) -o $(CLIENTE) $(LIBS)
	@echo "✅ Cliente compilado -> ./$(CLIENTE)"

$(SERVIDOR): $(SERVIDOR_SRC)
	@echo "🔧 Compilando servidor HTTP..."
	$(CC) $(CFLAGS) $(SERVIDOR_SRC) -o $(SERVIDOR) $(LIBS)
	@echo "✅ Servidor compilado -> ./$(SERVIDOR)"

run-server:
	@echo "🌐 Iniciando servidor na porta 8080..."
	@./$(SERVIDOR)

run-client:
	@echo "💻 Executando cliente para baixar arquivo..."
	@./$(CLIENTE) http://localhost:8080/foto.jpg

clean:
	@echo "🧹 Limpando binários..."
	rm -f $(CLIENTE) $(SERVIDOR)
