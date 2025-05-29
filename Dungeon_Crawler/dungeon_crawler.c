// ===================
// == BIBLIOTECAS ====
// ===================
#include <stdio.h>    // Entrada e saída padrão
#include <stdlib.h>   // Alocação de memória, geração de números aleatórios
#include <string.h>   // Manipulação de strings e memória
#include <ctype.h>    // Manipulação de caracteres (toupper)
#include <time.h>     // Controle de aleatoriedade (rand, srand)

// ==============================
// == SUPORTE CROSS-PLATAFORMA ==
// ==============================
#ifdef _WIN32
  #include <conio.h>                         // Para Windows - leitura de tecla
  #define CLEAR()   system("cls")             // Limpa a tela no Windows
  #define GETCH()   _getch()                  // Captura tecla no Windows
#else
  #include <termios.h>                        // Para Linux/Mac - manipulação de terminal
  #include <unistd.h>
  // Função que lê uma tecla sem esperar ENTER
  static int getch(void) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);           // Salva estado atual do terminal
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);         // Desabilita modo canonico e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Aplica novo estado
    int c = getchar();                        // Lê tecla
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restaura estado anterior
    return c;
  }
  #define CLEAR()   printf("\x1b[2J\x1b[H")    // Limpa tela no Linux/Mac
  #define GETCH()   getch()                   // Lê tecla
#endif

// ===================
// == CONSTANTES =====
// ===================
#define MAX_LINE 512      // Tamanho máximo de uma linha lida no mapa
#define MAX_TRIES 3       // Máximo de tentativas (vidas)

// ===================
// == VARIÁVEIS ======
// ===================
static int v_counter = 0; // Controla o movimento do monstro perseguidor 'V' (move a cada 2 turnos)
static int tries = 0;     // Contagem de tentativas (vale para o jogo inteiro)

// ===================
// == ESTRUTURAS =====
// ===================
// Estrutura de monstro
typedef struct {
    int i, j;    // Posição (linha, coluna)
    char type;   // Tipo ('X' = aleatório, 'V' = perseguidor)
} Monster;

// Estrutura do mapa
typedef struct {
    int rows, cols;      // Dimensões do mapa
    char **orig;         // Mapa original (paredes, portas, chave, sem monstros ou jogador)
    char **cells;        // Mapa dinâmico (em tempo real)
    Monster *mons;       // Vetor de monstros no mapa
    int monCount;        // Quantidade de monstros
    int startX, startY;  // Posição inicial do jogador
} Map;

// ===================
// == PROTÓTIPOS =====
// ===================
Map* load_map(const char *filename);
void free_map(Map *m);
void rebuild_cells(Map *m);
void draw_map(const Map *m);
void move_monsters(Map *m, int *px, int *py);
int  play_phase(const char *filename);
void show_menu(void);
void show_credits(void);
void show_exit(void);
void show_victory(void);
void show_defeat(void);

// ===================
// == FUNÇÃO MAIN ====
// ===================
int main(void) {
    srand((unsigned)time(NULL)); // Inicializa o gerador de números aleatórios
    while (1) {
        show_menu();              // Exibe menu
        char opt = GETCH();       // Captura opção

        if (opt == '1') {         // Jogar
            tries = 0;            // Reseta tentativas
            // Executa todas as fases. Se perder em alguma, volta ao menu.
            if (!play_phase("maps/village.txt") || !play_phase("maps/fase1.txt") ||
                !play_phase("maps/fase2.txt")   || !play_phase("maps/fase3.txt")) {
                show_defeat();
                continue;
            }
            show_victory();
        } else if (opt == '2') {  // Créditos
            show_credits();
        } else if (opt == '3') {  // Sair
            show_exit();
            break;
        }
    }
    return 0;
}

// ==========================
// == TELAS DO MENU ========
// ==========================

// Exibe o menu principal com as opções básicas do jogo
void show_menu(void) {
    CLEAR(); // Limpa a tela
    printf("=== DUNGEON CRAWLER ===\n");
    printf("1) Jogar\n2) Creditos\n3) Sair\n");
    printf("Escolha: ");
}

// Exibe os créditos do jogo
void show_credits(void) {
    CLEAR(); // Limpa a tela
    printf("=== CRÉDITOS ===\n");
    printf("- Vitor Lourenco, aluno de Ciencia da Computacao do Cesupa\n\n");
    printf("Pressione qualquer tecla para voltar...");
    GETCH(); // Aguarda tecla
}

// Exibe mensagem ao sair do jogo
void show_exit(void) {
    CLEAR();
    printf("Obrigado por jogar! Ate a proxima.\n");
}

// Tela de vitória ao fim do jogo
void show_victory(void) {
    CLEAR();
    printf("PARABENS! Voce venceu o jogo!\n");
    printf("Pressione qualquer tecla para voltar ao menu...");
    GETCH();
}

// Tela de derrota após exceder tentativas
void show_defeat(void) {
    CLEAR();
    printf("DERROTA! Voce ficou sem tentativas.\n");
    printf("Voltando ao menu principal...\n");
    GETCH();
}

// ========================================
// == CARREGAMENTO E MANIPULAÇÃO DE MAPA ==
// ========================================

// Carrega um mapa de um arquivo texto e o converte para uma estrutura Map
Map* load_map(const char *filename) {
    FILE *f = fopen(filename, "r"); // Abre o arquivo para leitura
    if (!f) {
        printf("Erro ao carregar mapa: %s\n", filename);
        GETCH(); // Aguarda o usuário antes de sair
        return NULL; // Falha ao abrir o arquivo
    }

    int R, C;
    // Lê o número de linhas (R) e colunas (C) do mapa
    if (fscanf(f, "%d %d\n", &R, &C) != 2) { fclose(f); return NULL; }

    char buf[MAX_LINE];
    char **raw = malloc(R * sizeof(char*)); // Aloca matriz para armazenar linhas lidas

    // Lê cada linha do mapa do arquivo
    for (int i = 0; i < R; i++) {
        raw[i] = malloc(C + 1); // +1 para caractere nulo
        if (!fgets(buf, MAX_LINE, f)) buf[0] = '\0';
        int len = strcspn(buf, "\r\n"); // Remove quebras de linha
        for (int j = 0; j < C; j++) 
            raw[i][j] = j < len ? buf[j] : ' '; // Preenche espaços vazios
    }
    fclose(f);

    // Identifica monstros e posição inicial do jogador
    int count = 0, sx = -1, sy = -1;
    for (int i = 0; i < R; i++) for (int j = 0; j < C; j++) {
        if (raw[i][j] == 'X' || raw[i][j] == 'V') count++;
        else if (raw[i][j] == '&') { sy = i; sx = j; }
    }

    // Aloca a struct Map e seus campos
    Map *m = malloc(sizeof(Map));
    m->rows = R;
    m->cols = C;
    m->startX = sx;
    m->startY = sy;
    m->monCount = count;
    m->mons = malloc(count * sizeof(Monster));
    m->orig = malloc(R * sizeof(char*));
    m->cells = malloc(R * sizeof(char*));

    // Copia o conteúdo bruto para a estrutura do mapa e posiciona os monstros
    int idx = 0;
    for (int i = 0; i < R; i++) {
        m->orig[i] = malloc(C);
        for (int j = 0; j < C; j++) {
            char c = raw[i][j];
            if (c == 'X' || c == 'V') {
                m->mons[idx++] = (Monster){i, j, c}; // Salva o monstro
                m->orig[i][j] = ' '; // Deixa espaço vazio no mapa original
            }
            else if (c == '&') m->orig[i][j] = ' '; // Remove o jogador do mapa original
            else m->orig[i][j] = c; // Copia normalmente
        }
        m->cells[i] = malloc(C); // Aloca linha para o mapa em tempo real
        free(raw[i]); // Libera linha original lida
    }
    free(raw); // Libera matriz temporária

    rebuild_cells(m); // Reconstrói o mapa com monstros visíveis
    return m;
}

// Libera toda a memória alocada para o mapa
void free_map(Map *m) {
    for (int i = 0; i < m->rows; i++) {
        free(m->orig[i]); // Libera cada linha da matriz original
        free(m->cells[i]); // Libera cada linha da matriz em tempo real
    }
    free(m->orig);
    free(m->cells);
    free(m->mons);
    free(m); // Libera a struct Map
}

// Copia o conteúdo original para a matriz atual e reposiciona os monstros
void rebuild_cells(Map *m) {
    for (int i = 0; i < m->rows; i++)
        memcpy(m->cells[i], m->orig[i], m->cols); // Cópia simples da matriz base
    for (int k = 0; k < m->monCount; k++) {
        Monster *M = &m->mons[k];
        m->cells[M->i][M->j] = M->type; // Posiciona os monstros na célula atual
    }
}

// Desenha o mapa na tela com instruções de controle
void draw_map(const Map *m) {
    CLEAR(); // Limpa a tela
    for (int i = 0; i < m->rows; i++) {
        fwrite(m->cells[i], 1, m->cols, stdout); // Imprime linha por linha
        putchar('\n');
    }
    printf("\nUse W/A/S/D para mover, I para interagir.\n");
}
// ===================================
// == MOVIMENTAÇÃO DOS MONSTROS =====
// ===================================

// Responsável por mover todos os monstros no mapa a cada turno
void move_monsters(Map *m, int *px, int *py) {
    int R = m->rows, C = m->cols; // Dimensões do mapa

    for (int k = 0; k < m->monCount; k++) { // Para cada monstro
        Monster *M = &m->mons[k];
        int ni = M->i, nj = M->j; // Próximas posições do monstro

        if (M->type == 'X') { // Monstro aleatório
            int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}}; // Cima, baixo, esquerda, direita

            // Embaralha a ordem das direções (algoritmo Fisher-Yates)
            for (int a = 3; a > 0; a--) {
                int r = rand() % (a + 1);
                int t0 = dirs[a][0], t1 = dirs[a][1];
                dirs[a][0] = dirs[r][0]; dirs[a][1] = dirs[r][1];
                dirs[r][0] = t0; dirs[r][1] = t1;
            }

            // Tenta se mover em uma direção válida
            for (int d = 0; d < 4; d++) {
                int ti = M->i + dirs[d][0], tj = M->j + dirs[d][1];
                if (ti < 0 || ti >= R || tj < 0 || tj >= C) continue; // Fora do mapa
                char dest = m->cells[ti][tj];
                if (dest == ' ' || dest == '=' || (ti == *py && tj == *px)) {
                    ni = ti; nj = tj;
                    if (ti == *py && tj == *px) tries++; // Pegou o jogador
                    break;
                }
            }
        } else { // Monstro perseguidor 'V'
            v_counter++; // Contador global para limitar movimento
            if (v_counter % 2) continue; // Só se move a cada dois turnos

            // Calcula a direção mais próxima do jogador
            int di = *py - M->i, dj = *px - M->j;
            if (abs(di) >= abs(dj)) ni = M->i + (di > 0 ? 1 : -1);
            else nj = M->j + (dj > 0 ? 1 : -1);

            // Verifica se o novo destino é válido
            if (ni < 0 || ni >= R || nj < 0 || nj >= C ||
                (m->cells[ni][nj] != ' ' && m->cells[ni][nj] != '=' &&
                !(ni == *py && nj == *px))) {
                ni = M->i; nj = M->j; // Permanece parado
            }
            if (ni == *py && nj == *px) tries++; // Pegou o jogador
        }

        // Atualiza posição do monstro
        M->i = ni; M->j = nj;
    }

    rebuild_cells(m); // Atualiza o mapa com as novas posições dos monstros
    m->cells[*py][*px] = '&'; // Recoloca o jogador no mapa
}
// ===================================
// == LOOP PRINCIPAL DE CADA FASE ====
// ===================================

// Esta função executa o ciclo principal de uma fase do jogo
int play_phase(const char *filename) {
    Map *m = load_map(filename); // Carrega o mapa da fase
    if (!m) return 0; // Se falhar no carregamento, retorna derrota

    int px = m->startX, py = m->startY; // Posição inicial do jogador
    int hasKey = 0; // Flag que indica se o jogador pegou a chave

    while (1) {
        draw_map(m); // Desenha o mapa atualizado na tela
        printf("Chave: %s | Tentativas: %d/%d\n", hasKey ? "SIM" : "NAO", tries, MAX_TRIES);

        char cmd = toupper(GETCH()); // Lê o comando do jogador e transforma em maiúsculo
        int nx = px, ny = py; // Cria coordenadas futuras baseadas na direção do movimento

        if (cmd == 'W') ny--; // Cima
        else if (cmd == 'A') nx--; // Esquerda
        else if (cmd == 'S') ny++; // Baixo
        else if (cmd == 'D') nx++; // Direita

        else if (cmd == 'I') { // Comando de interação
            char obj = m->orig[py][px]; // Verifica o objeto atual da célula original
            if (obj == 'P') { // NPC
                printf("Tome cuidado com os X e principalmente com os V.\n");
                GETCH();
            } else if (obj == '@') { // Chave
                hasKey = 1;
                printf("Voce pegou a chave!\n");
                GETCH();
                // Abre todas as portas
                for (int i = 0; i < m->rows; i++)
                    for (int j = 0; j < m->cols; j++)
                        if (m->orig[i][j] == 'D') m->orig[i][j] = '=';
                rebuild_cells(m);
            } else if (obj == 'O') { // Botão
                printf("Voce ativou o botao! As paredes especiais desapareceram!\n");
                GETCH();
                // Remove paredes '-' do mapa
                for (int i = 0; i < m->rows; i++)
                    for (int j = 0; j < m->cols; j++)
                        if (m->orig[i][j] == '-') m->orig[i][j] = ' ';
                rebuild_cells(m);
            } else if ((obj == 'D' || obj == '=') && hasKey) {
                // Se estiver na porta e tiver a chave, fase concluída
                free_map(m);
                return 1;
            }
            move_monsters(m, &px, &py); // Move os monstros após a ação
            if (tries >= MAX_TRIES) { free_map(m); return 0; } // Verifica se perdeu
            continue;
        }

        // Verifica se nova posição é válida
        if (nx < 0 || nx >= m->cols || ny < 0 || ny >= m->rows) continue;
        char dest = m->cells[ny][nx];
        if (dest == '*') continue; // Parede sólida
        if (dest == '#') { // Espinho: penaliza tentativa
            if (++tries >= MAX_TRIES) { free_map(m); return 0; }
            continue;
        }
        if (dest == '>') { // Teleporte
            int tx = -1, ty = -1;
            for (int i = 0; i < m->rows; i++)
                for (int j = 0; j < m->cols; j++)
                    if (m->orig[i][j] == '>' && (i != ny || j != nx)) { tx = j; ty = i; break; }
            if (tx != -1) { nx = tx; ny = ty; } // Teleporta
        }
        if (dest == 'X' || dest == 'V') { // Monstro: penaliza tentativa
            if (++tries >= MAX_TRIES) { free_map(m); return 0; }
            continue;
        }

        // Atualiza posição do jogador
        px = nx; py = ny;
        rebuild_cells(m);
        m->cells[py][px] = '&';
        move_monsters(m, &px, &py); // Move monstros após ação
        if (tries >= MAX_TRIES) { free_map(m); return 0; }
    }
}
