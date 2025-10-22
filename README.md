# 🌐 Projeto: Servidor e Cliente HTTP em C

Este projeto foi desenvolvido como parte da disciplina de **Redes de Computadores**, com o objetivo de compreender o funcionamento do protocolo **HTTP** a partir de sua implementação básica utilizando a linguagem **C** e **sockets TCP**.

---

## 📘 Objetivo

O sistema consiste em dois programas:

1. **Servidor HTTP (`server.c`)** — responsável por receber requisições e responder com uma página HTML simples.
2. **Cliente HTTP (`client.c`)** — responsável por enviar uma requisição HTTP ao servidor e exibir a resposta recebida.

Com isso, é possível visualizar de forma prática como funciona o ciclo **requisição/resposta** entre cliente e servidor, base fundamental da comunicação web.

---

## ⚙️ Tecnologias Utilizadas

- **Linguagem:** C  
- **Bibliotecas padrão:**  
  - `<stdio.h>` — entrada e saída  
  - `<stdlib.h>` — controle de memória e saída  
  - `<string.h>` — manipulação de strings  
  - `<unistd.h>` — funções POSIX (read, write, close)  
  - `<arpa/inet.h>` — criação e manipulação de sockets TCP/IP  
- **Protocolo de comunicação:** HTTP 1.1  
- **Modelo de rede:** Cliente/Servidor via TCP  

## 🚀 Como Executar o Projeto

### 1️⃣ Compilação

Abra um terminal dentro da pasta do projeto e compile os dois programas:

```bash
gcc server.c -o server
gcc client.c -o client

./server

./client
