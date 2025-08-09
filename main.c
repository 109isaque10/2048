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
    int probability;
} Game;

void showMenu(Game *currentGame);

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

void freeMatrix(int **blocks, int n){ //Libera a matrix da memória
    for (int i = 0; i < n; i++)
        free(blocks[i]);
    free(blocks);
}

void copyIntMatrix(int **dest, int**source, int n) {
    for (int i=0; i<n; i++)
        memcpy(dest[i], source[i], sizeof(int) * n);
}

void newBlock(Game *currentGame) { //Criar um novo bloco aleátorio
    int x, y;
    do { //Enquanto o bloco de destino for diferente de 0 (Nulo), aleatorizar novamente
        x = rand() % currentGame->size, y = rand() % currentGame->size;
    } while (currentGame->board[x][y] != 0);
    currentGame->board[x][y] = rand() % 100 < currentGame->probability ? 2 : 4; //Quando o número for menor que a probabilidade entra o número 2, se não o 4
}

int moveUp(Game *currentGame) { //Mover os blocos para cima
    int moved = 0;
    for (int j = 0; j < currentGame->size; j++) {
        int newPos = 0;
        for (int i = 0; i < currentGame->size-1; i++) //Move todos os blocos para cima
            if (currentGame->board[i][j] != 0) {
                if (i != newPos) {
                    currentGame->board[newPos][j] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                    moved = 1;
                }
                newPos++;
            }

        for (int i = 0; i < currentGame->size-1; i++) //Combina os blocos vizinhos de cima para baixo
            if (currentGame->board[i][j] != 0 && currentGame->board[i][j] == currentGame->board[i+1][j]) {
                currentGame->board[i][j] *= 2;
                currentGame->player.score += currentGame->board[i][j];
                currentGame->board[i+1][j] = 0;
                moved = 1;
            }

        newPos = 0;
        for (int i = 0; i < currentGame->size-1; i++) //Move novamente os blocos para cima para preencher os vazios
            if (currentGame->board[i][j] != 0) {
                if (i != newPos) {
                    currentGame->board[newPos][j] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                    if (currentGame->board[newPos][j] == 256)
                        currentGame->player.undoMoves++;
                    else if (currentGame->board[newPos][j] == 512)
                        currentGame->player.changeMoves++;
                    else if (currentGame->board[newPos][j] == 2048)
                        currentGame->won = 1;
                }
                newPos++;
            }
    }
    return moved;
}

int moveDown(Game *currentGame) { //Move os blocos para baixo
    int moved = 0;
    for (int j = 0; j < currentGame->size; j++) {
        int newPos = currentGame->size-1;
        for (int i = currentGame->size-1; i >= 0; i--) //Move todos os blocos para baixo
            if (currentGame->board[i][j] != 0) {
                if (i != newPos) {
                    currentGame->board[newPos][j] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                    moved = 1;
                }
                newPos--;
            }

        for (int i = currentGame->size-1; i > 0; i--)
            if (currentGame->board[i][j] != 0 && currentGame->board[i][j] == currentGame->board[i-1][j]) { //Combina os blocos vizinhos de baixo para cima
                currentGame->board[i][j] *= 2;
                currentGame->player.score += currentGame->board[i][j];
                currentGame->board[i-1][j] = 0;
                moved = 1;
            }

        newPos = currentGame->size-1;
        for (int i = currentGame->size-1; i >= 0; i--) //Move novamente os blocos paraa baixo para preencher os vazios
            if (currentGame->board[i][j] != 0) {
                if (i != newPos) {
                    currentGame->board[newPos][j] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                    if (currentGame->board[newPos][j] == 256)
                        currentGame->player.undoMoves++;
                    else if (currentGame->board[newPos][j] == 512)
                        currentGame->player.changeMoves++;
                    else if (currentGame->board[newPos][j] == 2048)
                        currentGame->won = 1;
                }
                newPos--;
            }
    }
    return moved;
}

int moveLeft(Game *currentGame) { //Move os blocos para a esquerda
    int moved = 0;
    for (int i = 0; i < currentGame->size; i++) {
        int newPos = 0;
        for (int j=0; j < currentGame->size-1; j++) //Move todos os blocos para a esquerda
            if (currentGame->board[i][j] != 0) {
                if (j != newPos) {
                    currentGame->board[i][newPos] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                    moved = 1;
                }
                newPos++;
            }

        for (int j = 0; j < currentGame->size-1; j++) //Combina os blocos vizinhos da esquerda para a direita
            if (currentGame->board[i][j] != 0 && currentGame->board[i][j] == currentGame->board[i][j+1]) {
                currentGame->board[i][j] *= 2;
                currentGame->player.score += currentGame->board[i][j];
                currentGame->board[i][j+1] = 0;
                moved = 1;
            }

        newPos = 0;
        for (int j = 0; j < currentGame->size-1; j++) //Move novamente os blocos para preencher os vazios
            if (currentGame->board[i][j] != 0) {
                if (j != newPos) {
                    currentGame->board[i][newPos] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                    if (currentGame->board[i][newPos] == 256)
                        currentGame->player.undoMoves++;
                    else if (currentGame->board[i][newPos] == 512)
                        currentGame->player.changeMoves++;
                    else if (currentGame->board[i][newPos] == 2048)
                        currentGame->won = 1;
                }
                newPos++;
            }
    }
    return moved;
}

int moveRight(Game *currentGame) { //Move os blocos para a direita
    int moved = 0;
    for (int i = 0; i < currentGame->size; i++) { //Move todos blocos não nulos para a direita
        int newPos = currentGame->size-1;
        for (int j = currentGame->size-1; j >= 0; j--)
            if (currentGame->board[i][j] != 0) {
                if (j != newPos) {
                    currentGame->board[i][newPos] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                    moved = 1;
                }
                newPos--;
            }

        for (int j = currentGame->size-1; j > 0; j--) //Combina os blocos vizinhos da direita para a esquerda
            if (currentGame->board[i][j] != 0 && currentGame->board[i][j] == currentGame->board[i][j-1]) {
                currentGame->board[i][j] *= 2;
                currentGame->player.score += currentGame->board[i][j];
                currentGame->board[i][j-1] = 0;
                moved = 1;
            }

        newPos = currentGame->size-1;
        for (int j = currentGame->size-1; j >= 0; j--) //Move novamente os blocos para preencher os vazios
            if (currentGame->board[i][j] != 0) {
                if (j != newPos) {
                    currentGame->board[i][newPos] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                    if (currentGame->board[i][newPos] == 256)
                        currentGame->player.undoMoves++;
                    else if (currentGame->board[i][newPos] == 512)
                        currentGame->player.changeMoves++;
                    else if (currentGame->board[i][newPos] == 2048)
                        currentGame->won = 1;
                }
                newPos--;
            }
    }
    return moved;
}

void undoMove(Game *currentGame, int **tempBoard) {
    copyIntMatrix(currentGame->board, tempBoard, currentGame->size);
    currentGame->player.undoMoves--;
}

void changePos(Game *currentGame) {
    int x, y, i1=0, j1=0, i2=0, j2=0;
    char a, b;
    scanf(" %c%d", &a, &x);
    scanf(" %c%d", &b, &y);
    if (a<=90 && b<=90) {
        i1 = a - 'A';
        j1 = x-1;
        i2 = b - 'A';
        j2 = y-1;
    }else if (a<=122 && b<=122) {
        i1 = a - 'a';
        j1 = x-1;
        i2 = b - 'a';
        j2 = y-1;
    }

    if (currentGame->board[i1][j1] != 0 && currentGame->board[i2][j2] != 0) {
        const int tmpBlock = currentGame->board[i2][j2];
        currentGame->board[i2][j2] = currentGame->board[i1][j1];
        currentGame->board[i1][j1] = tmpBlock;
    }
}

void printTopTable(int n) { //Imprime o cabeçario da tabela
    printf(" " TAB_TL);
    for (int i=0;i<n;i++) {
        for (int j=0;j<=n;j++)
            printf(TAB_HOR);
        if (i!=n-1) //Coloca junção de tabela, exceto no final
            printf(TAB_TJ);
    }
    printf(TAB_TR "\n");
}

void printMiddleTable(int n) { //Imprime as divisões horizontais
    printf(" " TAB_ML);
    for (int i=0;i<n;i++) {
        for (int j=0;j<=n;j++)
            printf(TAB_HOR);
        if (i!=n-1) //Coloca junção de tabela, exceto no final
            printf(TAB_MJ);
    }
    printf(TAB_MR "\n");
}

void printBottomTable(int n) { //Imprime o final da tabela
    printf(" " TAB_BL);
    for (int i=0;i<n;i++) {
        for (int j=0;j<=n;j++)
            printf(TAB_HOR);
        if (i!=n-1) //Imprime junção da tabela, exceto no final
            printf(TAB_BJ);
    }
    printf(TAB_BR "\n ");

    for (int i=0;i<n;i++)
        if (i==0)
            printf("%*d", n, i+1);
        else
            printf("%*d", n+2, i+1);
    printf("\n");
}

void printGame(int **block, int n) { //Imprime a tabela completa do jogo
    clear();
    printTopTable(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j == 0)
                printf("%c", 'A'+i);
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

void printScore(Player player) {
    printf("Pontuação: %4d Desfazer: %2d Trocar: %2d\n", player.score, player.undoMoves, player.undoMoves);
}

void saveGame(const Game *currentGame) {
    char fileName[TAM];

    clear();
    printf("Qual o nome do arquivo a ser salvo?\n(ATENÇÃO! O ARQUIVO SERÁ SOBRESCRITO)\n");
    scanf("%s", fileName);

    FILE *file = fopen(fileName, "wb");
    fwrite(&currentGame->size, sizeof(int), 1, file);
    fwrite(&currentGame->probability, sizeof(int), 1, file);
    fwrite(&currentGame->player, sizeof(Player), 1, file);
    fwrite(&currentGame->won, sizeof(int), 1, file);

    for (int i=0; i<currentGame->size; i++)
        for (int j=0; j<currentGame->size; j++)
            fwrite(&currentGame->board[i][j], sizeof(int), 1, file);

    fclose(file);
}

void loadGame(Game *currentGame) {
    char fileName[TAM];

    clear();
    printf("Qual o nome do arquivo a ser carregado?\n");
    scanf("%s", fileName);

    FILE *file = fopen(fileName, "rb");
    fread(&currentGame->size, sizeof(int), 1, file);
    fread(&currentGame->probability, sizeof(int), 1, file);
    fread(&currentGame->player, sizeof(Player), 1, file);
    fread(&currentGame->won, sizeof(int), 1, file);

    currentGame->board = createMatrix(currentGame->size, currentGame->size);

    for (int i=0; i<currentGame->size; i++)
        for (int j=0; j<currentGame->size; j++)
            fread(&currentGame->board[i][j], sizeof(int), 1, file);

    fclose(file);
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
    switch (currentGame->size) { //Determina a probabilidade de acordo com a tabela escolhida
        case 4:
            currentGame->probability = 90;
            break;
        case 5:
            currentGame->probability = 85;
            break;
        default:
            currentGame->probability = 80;
            break;
    }
    newBlock(currentGame);
}

void runGame(Game *currentGame) {
    int moved = 0; //Variavel que determina se alguma peça foi movida
    int **tempBoard = createMatrix(currentGame->size, currentGame->size);
    while (!currentGame->won) {
        if (moved) {
            newBlock(currentGame);
            moved = 0;
        }
        printGame(currentGame->board, currentGame->size);
        printScore(currentGame->player);
        char choice;
        scanf(" %c", &choice);
        switch (choice) { //Determina o que fazer de acordo com o que o usuário digitar
            case 'a':
            case 'A':
                copyIntMatrix(tempBoard, currentGame->board, currentGame->size);
                moved = moveLeft(currentGame);
                break;
            case 'd':
            case 'D':
                copyIntMatrix(tempBoard, currentGame->board, currentGame->size);
                moved = moveRight(currentGame);
                break;
            case 's':
            case 'S':
                copyIntMatrix(tempBoard, currentGame->board, currentGame->size);
                moved = moveDown(currentGame);
                break;
            case 'w':
            case 'W':
                copyIntMatrix(tempBoard, currentGame->board, currentGame->size);
                moved = moveUp(currentGame);
                break;
            case 'u':
            case 'U':
                undoMove(currentGame, tempBoard);
                break;
            case 't':
            case 'T':
                changePos(currentGame);
                break;
            case 'v':
            case 'V':
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

int exitGame(Game *currentGame) { //Confirmação simples de sair do jogo
    clear();
    while (1){ //Repete até uma opção válida ser digitada
        printf("Você realmente deseja sair? (Sim/Não)\n");

        char choice[3];
        scanf("%s", choice);

        switch (choice[0]) {
            //Se for sair não faz nada e volta ao main()
            case 's':
            case 'S':
                return 1;
            case 'n':
            case 'N':
                return 0;
            default:
                printf("\nOpção inválida!\n");
                exitGame(currentGame);
        }
    }
}

void showMenu(Game *currentGame) {
    while (1){
        //Mostra o menu inicial
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
                if (exitGame(currentGame))
                    return;
                else
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
                loadGame(currentGame);
                runGame(currentGame);
                break;
            case 'S':
            case 's':
                saveGame(currentGame);
                break;
            case 'M':
            case 'm':
                showRanking(currentGame);
                break;
            case 'A':
            case 'a':
                showHelp(currentGame);
                break;
            default:
                printf("\nOpção inválida!");
                break;
        }
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