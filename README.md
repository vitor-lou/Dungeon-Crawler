
# 🗺️ Dungeon Crawler – A Jornada do Herói

Um jogo de **aventura e puzzle em terminal**, desenvolvido em **C**, onde você enfrenta calabouços, monstros e armadilhas para escapar vitorioso.

---

## 📖 História do Jogo

> Em um vilarejo esquecido, antigos calabouços guardam mistérios, armadilhas e criaturas sombrias.  
> Você, um aventureiro destemido, deve atravessar esses perigos, encontrar a chave mágica e escapar pela porta antes que os monstros o alcancem.  
> Sua coragem definirá o futuro desse lugar!

---

## 🎯 Objetivo

- 🗝️ **Coletar a chave** (`@`)
- 🚪 **Abrir a porta** (`D`)
- 💀 **Sobreviver aos perigos** com **3 tentativas no total**, válidas para todas as fases

---

## 🕹️ Controles

| Tecla | Ação                       |
|-------|-----------------------------|
| `W`   | Mover para cima             |
| `A`   | Mover para esquerda         |
| `S`   | Mover para baixo            |
| `D`   | Mover para direita          |
| `I`   | **Interagir** (com NPC, botão, chave, porta) |

---

## 🧠 Elementos do Mapa

| Símbolo | Função                                          |
|---------|-------------------------------------------------|
| `&`     | Jogador                                         |
| `P`     | NPC (dá dicas)                                  |
| `@`     | Chave (necessária para abrir a porta)           |
| `D`     | Porta fechada                                   |
| `=`     | Porta aberta                                    |
| `O`     | Botão (ativa passagens secretas `-`)            |
| `#`     | Armadilha (perde 1 tentativa)                   |
| `>`     | Teleporte (leva a outro `>`)                    |
| `X`     | Monstro aleatório                               |
| `V`     | Monstro perseguidor (move a cada 2 turnos)      |

---

## ⚠️ Regras

- ☠️ Espinhos (`#`) → Perde 1 tentativa
- 👾 Monstros (`X` e `V`) → Perde 1 tentativa se encostar
- 🔑 Porta (`D`) só abre se tiver a chave (`@`)
- 🗝️ Botões (`O`) liberam passagens escondidas (`-`)
- 🚪 Teleportes (`>`) levam para outro teleporte do mapa
- 🛑 Se perder as **3 tentativas**, o jogo retorna ao menu principal

---

## 🔧 Instalação e Execução

### ✅ Pré-requisitos:
- Compilador C (`gcc`, `clang`, `tcc` ou similar)
- Terminal (Linux, macOS ou Windows)

### 🏗️ Compilar:
```bash
gcc src/dungeon_crawler.c -o dungeon_crawler
````

### ▶️ Executar:

No Linux/macOS:

```bash
./dungeon_crawler
```

No Windows:

```bash
dungeon_crawler.exe
```

---

## 👨‍💻 Créditos

| Nome                  | Função                              |
| --------------------- | ----------------------------------- |
| **Vitor Lourenço**    | Desenvolvedor e Game Designer       |

---

## 🌟 Roadmap Futuro

* ✔️ IA dos monstros (`X` e `V`) funcionando
* ✔️ Sistema de teleporte
* ⏳ Sistema de pontuação
* ⏳ Animações textuais no terminal
* ⏳ Salvamento de progresso
* ⏳ Modo hardcore (1 tentativa apenas)

---

## 📜 Licença

Este projeto é de **código aberto**, livre para estudo, uso, modificação e contribuições.
Sinta-se livre para contribuir, melhorar e compartilhar!

---

> 🚀 Feito com muita paixão, café e várias linhas de código em C.

```
