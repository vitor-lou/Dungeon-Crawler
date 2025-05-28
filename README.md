# Dungeon Crawler (Jogo em C)

🎮 Um jogo de aventura/puzzle em C, baseado em mapas de texto.  
O objetivo é coletar a chave, desviar de monstros e armadilhas, e abrir a porta para vencer cada fase.

## 📜 Descrição

Este é um jogo estilo **Dungeon Crawler** em modo texto, desenvolvido em linguagem **C**.  
Possui um tutorial e 3 fases. O jogador precisa se movimentar pelo mapa, pegar a chave (`@`), desviar de monstros e armadilhas, e abrir a porta (`D`) para completar a fase.

### 🧠 Mecânicas do jogo:

- `&` → Jogador  
- `P` → NPC (Dá dicas)  
- `@` → Chave  
- `D` → Porta fechada  
- `=` → Porta aberta  
- `O` → Botão que ativa passagem secreta (remove paredes `-`)  
- `#` → Armadilha de espinhos (perde 1 tentativa)  
- `>` → Teleporte (leva a outro ponto com `>`)  
- `X` → Monstro que anda de forma aleatória  
- `V` → Monstro perseguidor (segue o jogador, move a cada 2 turnos)

O jogador tem **3 tentativas** no total, **válidas para todas as fases**.

---

## 🚀 Como rodar o jogo

### ✅ Pré-requisitos:

- Compilador C (GCC, clang, etc.)
- Terminal (Windows, Linux ou Mac)

### 🏗️ Compilar:

No terminal, execute:

```bash
gcc dungeon_crawler.c -o dungeon_crawler
