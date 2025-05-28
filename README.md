🗺️ Dungeon Crawler – A Jornada do Herói
Um jogo de aventura e puzzle em terminal, desenvolvido em C, onde você embarca em uma jornada por calabouços sombrios, enfrentando monstros, armadilhas e desafios, enquanto busca a chave mágica para escapar.

📖 História do Jogo
Há muito tempo, um vilarejo foi tomado por forças sombrias.
Antigas catacumbas escondem mistérios, segredos e perigos.
Você, um aventureiro destemido, é a única esperança para restaurar a paz.

Seu objetivo é simples... na teoria:
⚔️ Explorar os calabouços, pegar a chave mágica e abrir a porta antes que os monstros o alcancem.

Mas não se engane. Pelo caminho, você encontrará armadilhas mortais, monstros imprevisíveis e corredores cheios de segredos.

🕹️ Regras do Jogo
🎯 Objetivo
🗝️ Pegue a chave (@)

🚪 Abra a porta (D)

💀 Sobreviva aos desafios com 3 tentativas no total, válidas para todas as fases

🔍 Elementos do Mapa
Símbolo	Função
&	Você (jogador)
P	NPC (fornece dicas quando interagido)
@	Chave (necessária para abrir portas)
D	Porta fechada (só abre com chave)
=	Porta aberta (quando já possui a chave)
O	Botão (ativa passagens secretas, remove -)
#	Armadilha de espinhos (perde 1 tentativa)
>	Teleporte (leva para outro >)
X	Monstro que anda de forma aleatória
V	Monstro perseguidor (move na direção do jogador a cada 2 turnos)

⚠️ Perigos
☠️ Armadilhas (#) → perde 1 tentativa ao pisar.

👾 Monstros (X e V) → perde 1 tentativa se te alcançarem.

❌ Se perder todas as tentativas (3 no total), volta ao menu principal.

🎮 Controles
Tecla	Ação
W	Mover para cima
A	Mover para esquerda
S	Mover para baixo
D	Mover para direita
I	Interagir (com NPC, botão, chave ou porta)

🔧 Como Instalar e Executar
✅ Pré-requisitos
Compilador C instalado (gcc, clang ou outro)

Um terminal (funciona no Windows, Linux e macOS)

🏗️ Compilar
Navegue até a pasta do projeto e execute:

bash
Copiar
Editar
gcc src/dungeon_crawler.c -o dungeon_crawler
▶️ Rodar o jogo
No Linux/macOS:

bash
Copiar
Editar
./dungeon_crawler
No Windows:

bash
Copiar
Editar
dungeon_crawler.exe
