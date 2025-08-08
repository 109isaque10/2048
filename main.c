//Isaque Victor
//25.1.4038
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TAM 50

#define ANSI_RESET            "\x1b[0m"  // desativa os efeitos anteriores
#define ANSI_BOLD             "\x1b[1m"  // coloca o texto em negrito
#define ANSI_COLOR_YELLOW     "\x1b[33m" // cor amarela
#define TAB_HOR "\u2501" // ━ (horizontal)
#define TAB_VER "\u2503" // ┃ (vertical)
#define TAB_TL  "\u250F" // ┏ (top-left)
#define TAB_ML  "\u2523" // ┣ (middle-left)
#define TAB_BL  "\u2517" // ┗ (bottom-left)
#define TAB_TJ  "\u2533" // ┳ (top-join)
#define TAB_MJ  "\u254B" // ╋ (middle-join)
#define TAB_BJ  "\u253B" // ┻ (bottom-join)
#define TAB_TR  "\u2513" // ┓ (top-right)
#define TAB_MR  "\u252B" // ┫ (middle-right)
#define TAB_BR  "\u251B" // ┛ (bottom-right)

typedef struct {
    char name[TAM];
    int score;
    int undoMoves;
    int changeMoves;
} Player;

typedef struct {
    Player player;
    int **board;
    int size;
    int won;
} Game;

void showMenu();

void clear() { //Limpa o terminal no linux e windows
    if(!system("clear") || !system("cls"))
        return;
}

int **createMatrix(const int n, const int m) { //Cria matriz com n e m
    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        matrix[i] = malloc(m*sizeof(int));
    return matrix;
}

void newBlock(int **block, const int n, const int probability) { //Criar um novo bloco aleátorio
    int x, y;
    do { //Enquanto o bloco de destino for diferente de 0 (Nulo), aleatorizar novamente
        x = rand() % n, y = rand() % n;
    } while (block[x][y] != 0);
    block[x][y] = rand() % 100 < probability ? 2 : 4; //Quando o número for menor que a probabilidade entra o número 2, se não o 4
}

int moveUp(int **block, int n, int *score) { //Mover os blocos para cima
    int moved = 0;
    for (int j = 0; j < n; j++) {
        int newPos = 0;
        for (int i = 0; i < n-1; i++) //Move todos os blocos para cima
            if (block[i][j] != 0) {
                if (i != newPos) {
                    block[newPos][j] = block[i][j];
                    block[i][j] = 0;
                    moved = 1;
                }
                newPos++;
            }

        for (int i = 0; i < n-1; i++) //Combina os blocos vizinhos de cima para baixo
            if (block[i][j] != 0 && block[i][j] == block[i+1][j]) {
                block[i][j] *= 2;
                *score += block[i][j];
                block[i+1][j] = 0;
                moved = 1;
            }

        newPos = 0;
        for (int i = 0; i < n-1; i++) //Move novamente os blocos para cima para preencher os vazios
            if (block[i][j] != 0) {
                if (i != newPos) {
                    block[newPos][j] = block[i][j];
                    block[i][j] = 0;
                }
                newPos++;
            }
    }
    return moved;
}

int moveDown(int **block, int n, int *score) { //Move os blocos para baixo
    int moved = 0;
    for (int j = 0; j < n; j++) {
        int newPos = n-1;
        for (int i = n-1; i >= 0; i--) //Move todos os blocos para baixo
            if (block[i][j] != 0) {
                if (i != newPos) {
                    block[newPos][j] = block[i][j];
                    block[i][j] = 0;
                    moved = 1;
                }
                newPos--;
            }

        for (int i = n-1; i > 0; i--)
            if (block[i][j] != 0 && block[i][j] == block[i-1][j]) { //Combina os blocos vizinhos de baixo para cima
                block[i][j] *= 2;
                *score += block[i][j];
                block[i-1][j] = 0;
                moved = 1;
            }

        newPos = n-1;
        for (int i = n-1; i >= 0; i--) //Move novamente os blocos paraa baixo para preencher os vazios
            if (block[i][j] != 0) {
                if (i != newPos) {
                    block[newPos][j] = block[i][j];
                    block[i][j] = 0;
                }
                newPos--;
            }
    }
    return moved;
}

int moveLeft(int **block, int n, int *score) { //Move os blocos para a esquerda
    int moved = 0;
    for (int i = 0; i < n; i++) {
        int newPos = 0;
        for (int j=0; j < n-1; j++) //Move todos os blocos para a esquerda
            if (block[i][j] != 0) {
                if (j != newPos) {
                    block[i][newPos] = block[i][j];
                    block[i][j] = 0;
                    moved = 1;
                }
                newPos++;
            }

        for (int j = 0; j < n-1; j++) //Combina os blocos vizinhos da esquerda para a direita
            if (block[i][j] != 0 && block[i][j] == block[i][j+1]) {
                block[i][j] *= 2;
                *score += block[i][j];
                block[i][j+1] = 0;
                moved = 1;
            }

        newPos = 0;
        for (int j = 0; j < n-1; j++) //Move novamente os blocos para preencher os vazios
            if (block[i][j] != 0) {
                if (j != newPos) {
                    block[i][newPos] = block[i][j];
                    block[i][j] = 0;
                }
                newPos++;
            }
    }
    return moved;
}

int moveRight(int **block, int n, int *score) { //Move os blocos para a direita
    int moved = 0;
    for (int i = 0; i < n; i++) { //Move todos blocos não nulos para a direita
        int newPos = n-1;
        for (int j = n-1; j >= 0; j--)
            if (block[i][j] != 0) {
                if (j != newPos) {
                    block[i][newPos] = block[i][j];
                    block[i][j] = 0;
                    moved = 1;
                }
                newPos--;
            }

        for (int j = n-1; j > 0; j--) //Combina os blocos vizinhos da direita para a esquerda
            if (block[i][j] != 0 && block[i][j] == block[i][j-1]) {
                block[i][j] *= 2;
                *score += block[i][j];
                block[i][j-1] = 0;
                moved = 1;
            }

        newPos = n - 1;
        for (int j = n-1; j >= 0; j--) //Move novamente os blocos para preencher os vazios
            if (block[i][j] != 0) {
                if (j != newPos) {
                    block[i][newPos] = block[i][j];
                    block[i][j] = 0;
                }
                newPos--;
            }
    }
    return moved;
}

void printTopTable(int n) { //Imprime o cabeçario da tabela
    printf(TAB_TL);
    for (int i=0;i<n;i++) {
        for (int j=0;j<=n;j++)
            printf(TAB_HOR);
        if (i!=n-1) //Coloca junção de tabela, exceto no final
            printf(TAB_TJ);
    }
    printf(TAB_TR "\n");
}

void printMiddleTable(int n) { //Imprime as divisões horizontais
    printf(TAB_ML);
    for (int i=0;i<n;i++) {
        for (int j=0;j<=n;j++)
            printf(TAB_HOR);
        if (i!=n-1) //Coloca junção de tabela, exceto no final
            printf(TAB_MJ);
    }
    printf(TAB_MR "\n");
}

void printBottomTable(int n) { //Imprime o final da tabela
    printf(TAB_BL);
    for (int i=0;i<n;i++) {
        for (int j=0;j<=n;j++)
            printf(TAB_HOR);
        if (i!=n-1) //Imprime junção da tabela, exceto no final
            printf(TAB_BJ);
    }
    printf(TAB_BR "\n");
}

void printGame(int **block, int n) { //Imprime a tabela completa do jogo
    clear();
    printTopTable(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (block[i][j] != 0) //Se o bloco for não nulo, colorir de amarelo
                printf( TAB_VER ANSI_BOLD ANSI_COLOR_YELLOW "%*d " ANSI_RESET, n, block[i][j]);
            else
                printf( TAB_VER ANSI_BOLD "%*d " ANSI_RESET, n, block[i][j]);
        }
        printf(TAB_VER "\n");
        if (i!=n-1) //No final imprime a divisão horizontal
            printMiddleTable(n);
    }
    printBottomTable(n);
}

void freeMatrix(int **blocks, int n){ //Libera a matrix da memória
    for (int i = 0; i < n; i++)
        free(blocks[i]);
    free(blocks);
}

void newGame(int n, char name[TAM], Game *currentGame) { //Cria um novo jogo
    int **blocks = createMatrix(n, n);
    clear();
    Player player = {"", 0, 0, 0};
    strcpy(player.name, name);
    currentGame->player = player;
    currentGame->size = n;
    currentGame->won = 0;
    currentGame->board = blocks;
}

void runGame(Game *currentGame) {
    int probability;
    switch (currentGame->size) { //Determina a probabilidade de acordo com a tabela escolhida
        case 4:
            probability = 90;
            break;
        case 5:
            probability = 85;
            break;
        default:
            probability = 80;
            break;
    }
    newBlock(currentGame->board, currentGame->size, probability);
    int moved = 0; //Variavel que determina se alguma peça foi movida
    while (!currentGame->won) {
        if (moved) {
            newBlock(currentGame->board, currentGame->size, probability);
            moved = 0;
        }
        printGame(currentGame->board, currentGame->size);
        char choice;
        scanf(" %c", &choice);
        switch (choice) { //Determina o que fazer de acordo com o que o usuário digitar
            case 'a':
            case 'A':
                moved = moveLeft(currentGame->board, currentGame->size, &currentGame->player.score);
                break;
            case 'd':
            case 'D':
                moved = moveRight(currentGame->board, currentGame->size, &currentGame->player.score);
                break;
            case 's':
            case 'S':
                moved = moveDown(currentGame->board, currentGame->size, &currentGame->player.score);
                break;
            case 'w':
            case 'W':
                moved = moveUp(currentGame->board, currentGame->size, &currentGame->player.score);
                break;
            case 'u':
            case 'U':
                //undoMove();
                break;
            case 't':
            case 'T':
                int pos1, pos2;
                scanf("%d%d", &pos1, &pos2);
                //changePos();
            case 'v':
            case 'V':
                showMenu();
                return;
        }
    }
}

void newGameMenu(Game *currentGame) { //Cria o menu de novo jogo
    clear();

    char name[TAM];
    printf("Digite seu nome: ");
    scanf(" %s", name);

    printf("\n\n(4) jogo padrão 4 x 4.\n");
    printf("(5) jogo 5 x 5.\n");
    printf("(6) jogo 6 x 6.\n");

    int choice;
    scanf(" %d", &choice);

    switch (choice) { //Caso o usuário digite algo inválido
        case 4:
        case 5:
        case 6:
            newGame(choice, name, currentGame);
            runGame(currentGame);
            return;
        default:
            printf("\nOpção inválida\n");
            newGameMenu(currentGame);
    }
}

void exitGame() { //Confirmação simples de sair do jogo
    clear();
    printf("Você realmente deseja sair? (Sim/Não)\n");

    char choice[3];
    scanf("%s", choice);

    switch (choice[0]) { //Se for sair não faz nada e volta ao main()
        case 's':
        case 'S':
            break;
        case 'n':
        case 'N':
            showMenu();
            return;
        default:
            printf("\nOpção inválida!\n");
            exitGame();
    }
}

void showMenu(Game *currentGame) { //Mostra o menu inicial
    clear();

    printf("(R) Sair\n");
    printf("(N) Novo Jogo\n");
    printf("(J) Continuar o jogo atual\n");
    printf("(C) Carregar um jogo salvo\n");
    printf("(S) Salvar o jogo atual\n");
    printf("(M) Mostrar Ranking\n");
    printf("(A) Ajuda com as instruções de como jogar\n");

    char choice;
    scanf(" %c", &choice);
    switch (choice) {
        case 'R':
        case 'r':
            exitGame();
            break;
        case 'N':
        case 'n':
            newGameMenu(currentGame);
            break;
        case 'J':
        case 'j':
            if (currentGame->board != NULL)
                runGame(currentGame);
            else {
                clear();
                printf("Nenhum jogo em andamento!");
            }
            break;
        case 'C':
        case 'c':
            showMenu(currentGame);
            break;
        case 'M':
        case 'm':
            system("clear");
            break;
        case 'A':
        case 'a':
            system("clear");
            return;
        default:
            printf("\nOpção inválida!");
            showMenu(currentGame);
    }
}

int main() {
    srand(time(NULL));
    Game currentGame = {.board = NULL};
    showMenu(&currentGame);
    if (currentGame.board != NULL)
        freeMatrix(currentGame.board, currentGame.size);
    return 0;
}