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
void show_menu(void) {
    CLEAR();
    printf("=== DUNGEON CRAWLER ===\n");
    printf("1) Jogar\n2) Creditos\n3) Sair\n");
    printf("Escolha: ");
}

void show_credits(void) {
    CLEAR();
    printf("=== CRÉDITOS ===\n");
    printf("- Vitor Lourenco, aluno de Ciencia da Computacao do Cesupa\n\n");
    printf("Pressione qualquer tecla para voltar...");
    GETCH();
}

void show_exit(void) {
    CLEAR();
    printf("Obrigado por jogar! Ate a proxima.\n");
}

void show_victory(void) {
    CLEAR();
    printf("PARABENS! Voce venceu o jogo!\n");
    printf("Pressione qualquer tecla para voltar ao menu...");
    GETCH();
}

void show_defeat(void) {
    CLEAR();
    printf("DERROTA! Voce ficou sem tentativas.\n");
    printf("Voltando ao menu principal...\n");
    GETCH();
}

// ========================================
// == CARREGAMENTO E MANIPULAÇÃO DE MAPA ==
// ========================================
Map* load_map(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Erro ao carregar mapa: %s\n", filename);
        GETCH();
        return NULL;
    }

    int R, C;
    if (fscanf(f, "%d %d\n", &R, &C) != 2) { fclose(f); return NULL; }

    char buf[MAX_LINE];
    char **raw = malloc(R * sizeof(char*));

    for (int i = 0; i < R; i++) {
        raw[i] = malloc(C + 1);
        if (!fgets(buf, MAX_LINE, f)) buf[0] = '\0';
        int len = strcspn(buf, "\r\n");
        for (int j = 0; j < C; j++) 
            raw[i][j] = j < len ? buf[j] : ' ';
    }
    fclose(f);

    int count = 0, sx = -1, sy = -1;
    for (int i = 0; i < R; i++) for (int j = 0; j < C; j++) {
        if (raw[i][j] == 'X' || raw[i][j] == 'V') count++;
        else if (raw[i][j] == '&') { sy = i; sx = j; }
    }

    Map *m = malloc(sizeof(Map));
    m->rows = R;
    m->cols = C;
    m->startX = sx;
    m->startY = sy;
    m->monCount = count;
    m->mons = malloc(count * sizeof(Monster));
    m->orig = malloc(R * sizeof(char*));
    m->cells = malloc(R * sizeof(char*));

    int idx = 0;
    for (int i = 0; i < R; i++) {
        m->orig[i] = malloc(C);
        for (int j = 0; j < C; j++) {
            char c = raw[i][j];
            if (c == 'X' || c == 'V') {
                m->mons[idx++] = (Monster){i, j, c};
                m->orig[i][j] = ' ';
            }
            else if (c == '&') m->orig[i][j] = ' ';
            else m->orig[i][j] = c;
        }
        m->cells[i] = malloc(C);
        free(raw[i]);
    }
    free(raw);

    rebuild_cells(m);
    return m;
}

void free_map(Map *m) {
    for (int i = 0; i < m->rows; i++) {
        free(m->orig[i]);
        free(m->cells[i]);
    }
    free(m->orig);
    free(m->cells);
    free(m->mons);
    free(m);
}

void rebuild_cells(Map *m) {
    for (int i = 0; i < m->rows; i++)
        memcpy(m->cells[i], m->orig[i], m->cols);
    for (int k = 0; k < m->monCount; k++) {
        Monster *M = &m->mons[k];
        m->cells[M->i][M->j] = M->type;
    }
}

void draw_map(const Map *m) {
    CLEAR();
    for (int i = 0; i < m->rows; i++) {
        fwrite(m->cells[i], 1, m->cols, stdout);
        putchar('\n');
    }
    printf("\nUse W/A/S/D para mover, I para interagir.\n");
}

// ===================================
// == MOVIMENTAÇÃO DOS MONSTROS =====
// ===================================
void move_monsters(Map *m, int *px, int *py) {
    int R = m->rows, C = m->cols;

    for (int k = 0; k < m->monCount; k++) {
        Monster *M = &m->mons[k];
        int ni = M->i, nj = M->j;

        if (M->type == 'X') { // Monstro aleatório
            int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
            // Embaralha direções
            for (int a = 3; a > 0; a--) {
                int r = rand() % (a + 1);
                int t0 = dirs[a][0], t1 = dirs[a][1];
                dirs[a][0] = dirs[r][0]; dirs[a][1] = dirs[r][1];
                dirs[r][0] = t0; dirs[r][1] = t1;
            }
            // Tenta mover para uma direção válida
            for (int d = 0; d < 4; d++) {
                int ti = M->i + dirs[d][0], tj = M->j + dirs[d][1];
                if (ti < 0 || ti >= R || tj < 0 || tj >= C) continue;
                char dest = m->cells[ti][tj];
                if (dest == ' ' || dest == '=' || (ti == *py && tj == *px)) {
                    ni = ti; nj = tj;
                    if (ti == *py && tj == *px) tries++;
                    break;
                }
            }
        } else { // Monstro perseguidor 'V'
            v_counter++;
            if (v_counter % 2) continue; // Move a cada 2 turnos

            int di = *py - M->i, dj = *px - M->j;
            if (abs(di) >= abs(dj)) ni = M->i + (di > 0 ? 1 : -1);
            else nj = M->j + (dj > 0 ? 1 : -1);

            if (ni < 0 || ni >= R || nj < 0 || nj >= C ||
                (m->cells[ni][nj] != ' ' && m->cells[ni][nj] != '=' &&
                !(ni == *py && nj == *px))) {
                ni = M->i; nj = M->j;
            }
            if (ni == *py && nj == *px) tries++;
        }

        M->i = ni; M->j = nj;
    }
    rebuild_cells(m);
    m->cells[*py][*px] = '&'; // Atualiza posição do jogador
}

// ===================================
// == LOOP PRINCIPAL DE CADA FASE ====
// ===================================
int play_phase(const char *filename) {
    Map *m = load_map(filename);
    if (!m) return 0;

    int px = m->startX, py = m->startY; // Posição inicial do jogador
    int hasKey = 0; // Flag se pegou a chave

    while (1) {
        draw_map(m);
        printf("Chave: %s | Tentativas: %d/%d\n", hasKey ? "SIM" : "NAO", tries, MAX_TRIES);

        char cmd = toupper(GETCH());
        int nx = px, ny = py;

        if (cmd == 'W') ny--;
        else if (cmd == 'A') nx--;
        else if (cmd == 'S') ny++;
        else if (cmd == 'D') nx++;

        else if (cmd == 'I') {
            char obj = m->orig[py][px];
            if (obj == 'P') {
                printf("Tome cuidado com os X e principalmente com os V.\n");
                GETCH();
            } else if (obj == '@') {
                hasKey = 1;
                printf("Voce pegou a chave!\n");
                GETCH();
                for (int i = 0; i < m->rows; i++)
                    for (int j = 0; j < m->cols; j++)
                        if (m->orig[i][j] == 'D') m->orig[i][j] = '=';
                rebuild_cells(m);
            } else if (obj == 'O') {
                printf("Voce ativou o botao! As paredes especiais desapareceram!\n");
                GETCH();
                for (int i = 0; i < m->rows; i++)
                    for (int j = 0; j < m->cols; j++)
                        if (m->orig[i][j] == '-') m->orig[i][j] = ' ';
                rebuild_cells(m);
            } else if ((obj == 'D' || obj == '=') && hasKey) {
                free_map(m);
                return 1;
            }
            move_monsters(m, &px, &py);
            if (tries >= MAX_TRIES) { free_map(m); return 0; }
            continue;
        }

        if (nx < 0 || nx >= m->cols || ny < 0 || ny >= m->rows) continue;
        char dest = m->cells[ny][nx];
        if (dest == '*') continue;
        if (dest == '#') { if (++tries >= MAX_TRIES) { free_map(m); return 0; } continue; }
        if (dest == '>') {
            int tx = -1, ty = -1;
            for (int i = 0; i < m->rows; i++)
                for (int j = 0; j < m->cols; j++)
                    if (m->orig[i][j] == '>' && (i != ny || j != nx)) { tx = j; ty = i; break; }
            if (tx != -1) { nx = tx; ny = ty; }
        }
        if (dest == 'X' || dest == 'V') { if (++tries >= MAX_TRIES) { free_map(m); return 0; } continue; }

        px = nx; py = ny;
        rebuild_cells(m);
        m->cells[py][px] = '&';
        move_monsters(m, &px, &py);
        if (tries >= MAX_TRIES) { free_map(m); return 0; }
    }
}
