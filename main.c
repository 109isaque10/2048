//Isaque Victor
//25.1.4038
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TAM 27

#define ANSI_RESET            "\x1b[0m"  // desativa os efeitos anteriores
#define ANSI_BOLD             "\x1b[1m"  // coloca o texto em negrito
#define ANSI_ITALIC           "\x1b[3m" // coloca o texto em italico
#define ANSI_UNDERLINE        "\x1b[4m" // coloca double underline no texto
#define ANSI_DOUBLE_UNDERLINE        "\x1b[21m" // coloca double underline no texto
#define ANSI_COLOR_YELLOW     "\x1b[33m" // cor amarela
#define ANSI_COLOR_RED        "\x1b[31m" // cor vermelha
#define ANSI_COLOR_GREEN      "\x1b[32m" // cor verde
#define ANSI_COLOR_BLUE       "\x1b[34m" // cor azul
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
    Player player[10];
    int playerCount;
} Ranking;

typedef struct {
    int lastScore;
    int **lastBoard;
} LastMove;

typedef struct {
    Player player;
    int **board;
    int size;
    int won;
    int probability;
} Game;

void clear() { //Limpa o terminal no linux e windows
    if(!system("clear") || !system("cls"))
        return;
}

void enterContinue() {
    printf("\nPressione enter para continuar...");
    getchar();
    while( getchar() != '\n' );
}

int **createMatrix(const int n, const int m) { //Cria matriz com n e m
    int **matrix = calloc(n, sizeof(int *)); //Usa calloc para inicializar os blocos com 0
    for (int i = 0; i < n; i++)
        matrix[i] = calloc(m, sizeof(int));
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

void newBlock(const Game *currentGame) { //Cria um novo bloco aleátorio
    int x, y;
    do { //Enquanto o bloco de destino for diferente de 0 (Nulo), aleatorizar novamente
        x = rand() % currentGame->size, y = rand() % currentGame->size;
    } while (currentGame->board[x][y] != 0);
    currentGame->board[x][y] = rand() % 100 < currentGame->probability ? 2 : 4; //Quando o número for menor que a probabilidade entra o número 2, se não o 4
}

int gameOver(const Game *currentGame) { //Testa todas as peças para ver se tem peças adjacentes que podem ser juntadas
    for (int i=0; i < currentGame->size; i++)
        for (int j=0; j < currentGame->size; j++) {
            if (currentGame->board[i][j] == 0)
                return 0;
            if (i!= 0 && currentGame->board[i][j] == currentGame->board[i-1][j])
                return 0;
            if (i<currentGame->size-1 && currentGame->board[i][j] == currentGame->board[i+1][j])
                return 0;
            if (j!= 0 && currentGame->board[i][j] == currentGame->board[i][j-1])
                return 0;
            if (j<currentGame->size-1 && currentGame->board[i][j] == currentGame->board[i][j+1])
                return 0;
        }
    return 1;
}

int moveUp(Game *currentGame) { //Mover os blocos para cima
    int moved = 0;
    for (int j = 0; j < currentGame->size; j++) {
        int newPos = 0;
        for (int i = 0; i < currentGame->size; i++) //Move todos os blocos para cima
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
                switch (currentGame->board[i][j]) {
                    case 256:
                        currentGame->player.undoMoves++;
                        break;
                    case 512:
                        currentGame->player.changeMoves++;
                        break;
                    case 2048:
                        currentGame->won = 1;
                }
            }

        newPos = 0;
        for (int i = 0; i < currentGame->size-1; i++) //Move novamente os blocos para cima para preencher os vazios
            if (currentGame->board[i][j] != 0) {
                if (i != newPos) {
                    currentGame->board[newPos][j] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
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
                switch (currentGame->board[i][j]) {
                    case 256:
                        currentGame->player.undoMoves++;
                        break;
                    case 512:
                        currentGame->player.changeMoves++;
                        break;
                    case 2048:
                        currentGame->won = 1;
                }
            }

        newPos = currentGame->size-1;
        for (int i = currentGame->size-1; i >= 0; i--) //Move novamente os blocos paraa baixo para preencher os vazios
            if (currentGame->board[i][j] != 0) {
                if (i != newPos) {
                    currentGame->board[newPos][j] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
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
        for (int j=0; j < currentGame->size; j++) //Move todos os blocos para a esquerda
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
                switch (currentGame->board[i][j]) {
                    case 256:
                        currentGame->player.undoMoves++;
                        break;
                    case 512:
                        currentGame->player.changeMoves++;
                        break;
                    case 2048:
                        currentGame->won = 1;
                }
            }

        newPos = 0;
        for (int j = 0; j < currentGame->size-1; j++) //Move novamente os blocos para preencher os vazios
            if (currentGame->board[i][j] != 0) {
                if (j != newPos) {
                    currentGame->board[i][newPos] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
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
                switch (currentGame->board[i][j]) {
                    case 256:
                        currentGame->player.undoMoves++;
                        break;
                    case 512:
                        currentGame->player.changeMoves++;
                        break;
                    case 2048:
                        currentGame->won = 1;
                }
            }

        newPos = currentGame->size-1;
        for (int j = currentGame->size-1; j >= 0; j--) //Move novamente os blocos para preencher os vazios
            if (currentGame->board[i][j] != 0) {
                if (j != newPos) {
                    currentGame->board[i][newPos] = currentGame->board[i][j];
                    currentGame->board[i][j] = 0;
                }
                newPos--;
            }
    }
    return moved;
}

void undoMove(Game *currentGame, LastMove *lastMove) { //Desfaz o ultimo movimento
    copyIntMatrix(currentGame->board, lastMove->lastBoard, currentGame->size);
    currentGame->player.score = lastMove->lastScore;
    currentGame->player.undoMoves--;
}

void changePos(const Game *currentGame) { //Muda a posição com o formato a1 b2
    int x, y, i1=0, j1=0, i2=0, j2=0;
    char a, b;
    scanf(" %c%d", &a, &x);
    scanf(" %c%d", &b, &y);
    if (a<=122 && b<=122) { //Usa os ASCII codes para determinar o valor de A na tabela, 'A'-'A'=0, 'B'-'A'=1..
        i1 = a - 'a';
        j1 = x-1;
        i2 = b - 'a';
        j2 = y-1;
    }else if (a<=90 && b<=90) {
        i1 = a - 'A';
        j1 = x-1;
        i2 = b - 'A';
        j2 = y-1;
    }

    if (currentGame->board[i1][j1] != 0 && currentGame->board[i2][j2] != 0) {
        const int tmpBlock = currentGame->board[i2][j2];
        currentGame->board[i2][j2] = currentGame->board[i1][j1];
        currentGame->board[i1][j1] = tmpBlock;
    } else {
        printf("\nVocê digitou uma posição inválida!");
        enterContinue();
    }
}

void printTopTable(const int n) { //Imprime o cabeçário da tabela
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
            if (block[i][j] != 0) {
                //Se o bloco for não nulo, colorir de acordo com a pontuação
                if (block[i][j] < 128)
                    printf( TAB_VER ANSI_BOLD ANSI_COLOR_YELLOW "%*d " ANSI_RESET, n, block[i][j]);
                else if (block[i][j] < 512)
                    printf( TAB_VER ANSI_BOLD ANSI_COLOR_BLUE "%*d " ANSI_RESET, n, block[i][j]);
                else if (block[i][j] < 2048)
                    printf( TAB_VER ANSI_BOLD ANSI_COLOR_GREEN "%*d " ANSI_RESET, n, block[i][j]);
                else
                    printf( TAB_VER ANSI_BOLD ANSI_COLOR_RED "%*d " ANSI_RESET, n, block[i][j]);
            } else
                printf( TAB_VER ANSI_BOLD "%*c " ANSI_RESET, n, ' ');
        }
        printf(TAB_VER "\n");
        if (i!=n-1) //No final imprime a divisão horizontal
            printMiddleTable(n);
    }
    printBottomTable(n);
}

void printScore(Player player) {
    printf("Pontuação: " ANSI_ITALIC "%6d " ANSI_RESET, player.score);
    printf("Desfazer: " ANSI_ITALIC "%2d " ANSI_RESET, player.undoMoves);
    printf("Trocar: " ANSI_ITALIC "%2d\n" ANSI_RESET, player.changeMoves);
}

void showHelp() {
    clear();
    printf(ANSI_BOLD "Como Jogar " ANSI_ITALIC ANSI_COLOR_YELLOW "2048:\n" ANSI_RESET);
    printf(ANSI_BOLD "Objetivo:\n" ANSI_RESET);
    printf("Combine blocos com o mesmo número até formar o bloco 2048. Quanto mais combinações, maior sua pontuação!\n\n");
    printf(ANSI_BOLD "Pontuação:\n" ANSI_RESET);
    printf("Os valores das peças resultantes são somados a pontuação.\n");
    printf("A cada peça obtida com valor igual a 256, você ganha um movimento desfazer.\n");
    printf("E a cada peça com valor 512, você ganha um movimento trocar.\n\n");
    printf(ANSI_BOLD "Comandos:\n" ANSI_RESET);
    printf("W – Move os blocos para cima\n");
    printf("A – Move os blocos para a esquerda\n");
    printf("S – Move os blocos para baixo\n");
    printf("D – Move os blocos para a direita\n");
    printf("U – Desfaz o último movimento\n");
    printf("T – Troca a posição de dois blocos\n");
    printf("V – Volta ao menu principal\n");
    enterContinue();
}

void showRanking(Ranking ranking[3]) {
    clear();
    int isNull=0;
    for (int i=0;i<3;i++) {
        if (ranking[i].playerCount != 0) {
            printf(ANSI_BOLD ANSI_DOUBLE_UNDERLINE "%-28s" TAB_VER "%-8s" TAB_VER "TAM %d\n" ANSI_RESET, "NOME", "PTS", 4+i);
        }
        else {
            isNull++;
            continue;
        }
        for (int j=0;j<ranking[i].playerCount;j++)
            printf("%-28s" TAB_VER "%8d" TAB_VER "\n", ranking[i].player[j].name, ranking[i].player[j].score);
    }
    if (isNull==3)
        printf("Não existem jogadores no ranking!");
    enterContinue();
}

void loadRanking(Ranking ranking[3]) { //Carrega o ranking de acordo com a formatação
    FILE *file = fopen("ranking.dat", "rb");
    if (file == NULL) {
        for (int i = 0; i < 3; i++) {
            ranking[i].playerCount = 0;
            for (int j = 0; j < 10; j++) {
                ranking[i].player[j].score = 0;
            }
        }
        return;
    }
    for (int i=0; i<3; i++)
        fread(&ranking[i].playerCount, sizeof(int), 1, file);
    for (int i=0; i<3; i++)
        for (int j=0; j<ranking[i].playerCount; j++) {
            fread(ranking[i].player[j].name, sizeof(char), TAM+1, file);
            fread(&ranking[i].player[j].score, sizeof(int), 1, file);
        }
    fclose(file);
}

void saveRanking(Game *currentGame, Ranking ranking[3]) {
    FILE *file = fopen("ranking.dat", "wb");
    int i=currentGame->size-4;
    for (int j=0; j<10; j++) //Eliminates lower scores
        if (currentGame->player.score > ranking[i].player[j].score) {
            for (int l=9; l>j; l--) //Lógica para armazenar os rankings em ordem crescente, e descartar os de menor pontuação
                ranking[i].player[l] = ranking[i].player[l-1];
            ranking[i].player[j] = currentGame->player;
            if (ranking[i].playerCount<10)
                ranking[i].playerCount++;
            break;
        }
    for (i=0; i<3; i++)
        fwrite(&ranking[i].playerCount, sizeof(int), 1, file);
    for (i=0; i<3; i++)
        for (int j=0; j<ranking[i].playerCount; j++) {
            fwrite(ranking[i].player[j].name, sizeof(char), TAM, file);
            fwrite(&ranking[i].player[j].score, sizeof(int), 1, file);
        }
    fclose(file);
}

void saveGame(const Game currentGame, const LastMove lastMove) { //Salva o jogo no formato pedido
    char fileName[TAM];

    clear();
    printf("Qual o nome do arquivo a ser salvo?\n(ATENÇÃO! O ARQUIVO SERÁ SOBRESCRITO)\n");
    scanf("%s", fileName);

    FILE *file = fopen(fileName, "w");
    fprintf(file, "%d %d %d", currentGame.size, currentGame.player.undoMoves, currentGame.player.changeMoves);
    fprintf(file, "\n%d %s\n", currentGame.player.score, currentGame.player.name);

    for (int i=0; i<currentGame.size; i++) {
        for (int j=0; j<currentGame.size; j++)
            fprintf(file, "%d ", currentGame.board[i][j]);
        fprintf(file, "\n");
    }

    for (int i=0; i<currentGame.size; i++) {
        for (int j=0; j<currentGame.size; j++)
            fprintf(file, "%d ", lastMove.lastBoard[i][j]); //Salva o último tabuleiro para poder desfazer
        fprintf(file, "\n");
    }

    fclose(file);
}

int loadGame(Game *currentGame, LastMove *lastMove) {
    char fileName[TAM];

    clear();
    printf("Qual o nome do arquivo a ser carregado?\n");
    scanf("%s", fileName);

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("O arquivo não existe!");
        enterContinue();
        return 1;
    }
    fscanf(file, "%d %d %d", &currentGame->size, &currentGame->player.undoMoves, &currentGame->player.changeMoves);
    fscanf(file, "%d", &currentGame->player.score);
    fgetc(file); //Elimina o espaço antes do nome
    fread(currentGame->player.name, sizeof(char), TAM, file); //Se usar fgets ele não consegue ler os 27 chars
    currentGame->player.name[strcspn(currentGame->player.name, "\n")] = '\0'; //Caso tenha, substitui o \n por \0

    currentGame->board = createMatrix(currentGame->size, currentGame->size);
    lastMove->lastBoard = createMatrix(currentGame->size, currentGame->size);

    for (int i=0; i<currentGame->size; i++)
        for (int j=0; j<currentGame->size; j++)
            fscanf(file, "%d", &currentGame->board[i][j]);

    for (int i=0; i<currentGame->size; i++)
        for (int j=0; j<currentGame->size; j++)
            fscanf(file, "%d", &lastMove->lastBoard[i][j]);

    fclose(file);
    return 0;
}

void newGame(const int n, char name[TAM], Game *currentGame, LastMove *lastMove) { //Cria um novo jogo
    int **blocks = createMatrix(n, n);
    lastMove->lastBoard = createMatrix(n, n);
    clear();
    Player player = {"", 0, 0, 0};
    name[strcspn(name, "\n")] = '\0'; //Elimina o \n final da string e coloca \0
    strcpy(player.name, name);
    currentGame->player = player;
    currentGame->size = n;
    currentGame->won = 0;
    currentGame->board = blocks;
    switch (currentGame->size) { //Determina a probabilidade conforme a tabela escolhida
        case 4:
            currentGame->probability = 90;
            break;
        case 5:
            currentGame->probability = 85;
            break;
        default:
            currentGame->probability = 80;
    }
    newBlock(currentGame);
    newBlock(currentGame);
}

void reset(Game *currentGame, LastMove *lastMove) { //Libera as matrizes e reseta o tabuleiro
    freeMatrix(currentGame->board, currentGame->size);
    freeMatrix(lastMove->lastBoard, currentGame->size);
    lastMove->lastBoard = NULL;
    currentGame->board = NULL;
}

void runGame(Game *currentGame, Ranking ranking[3], LastMove *lastMove) {
    int moved = 0, undid = 0, wonTold=0; /*Variável que determina se alguma peça foi movida,
    e variável auxiliar para evitar desfazer duas vezes, e outra para não repetir a mensagem de ganhou*/
    char choice; int valid=0;
    while (1) {
        if (moved) { //Se ele se moveu não se deve colocar um novo bloco
            newBlock(currentGame);
            undid=0;
            moved=0;
        }
        printGame(currentGame->board, currentGame->size);
        printScore(currentGame->player);
        if (gameOver(currentGame)) {
            printf("\n%s você perdeu!", currentGame->player.name);
            if (currentGame->player.undoMoves>0) {
                printf("\nVocê deseja desfazer um movimento ou voltar ao menu? (u/v)\n");
                do {
                    valid=0;
                    scanf(" %c", &choice);
                    switch (choice) {
                        case 'u':
                        case 'U':
                            undoMove(currentGame, lastMove);
                            undid=1;
                            valid=1;
                            continue;
                        case 'v':
                        case 'V':
                            return;
                        default:
                            printf("\nOpção inválida!\n");
                            printf("Digite uma opção: ");
                    }
                } while (!valid);
            }
            else {
                enterContinue(); //Se não tem movimentos desfazer disponíveis não precisa perguntar se quer voltar
                break;
            }
        }
        copyIntMatrix(lastMove->lastBoard, currentGame->board, currentGame->size);
        lastMove->lastScore = currentGame->player.score;
        do { //Continua a ler até achar um valor válido
            printf("\nDigite um comando: ");
            scanf(" %c", &choice);
            valid=0;
            switch (choice) {
                //Determina o que fazer de acordo com o que o usuário digitar
                case 'a':
                case 'A':
                    moved = moveLeft(currentGame);
                    valid=1;
                    break;
                case 'd':
                case 'D':
                    moved = moveRight(currentGame);
                    valid=1;
                    break;
                case 's':
                case 'S':
                    moved = moveDown(currentGame);
                    valid=1;
                    break;
                case 'w':
                case 'W':
                    moved = moveUp(currentGame);
                    valid=1;
                    break;
                case 'u':
                case 'U':
                    if (!undid && currentGame->player.undoMoves > 0) { //Desfaz o movimento
                        undoMove(currentGame, lastMove);
                        undid=1;
                    } else if (undid) {
                        printf("Você não pode voltar duas vezes seguidas!");
                        enterContinue();
                    } else {
                        printf("Você não pode desfazer ainda!");
                        enterContinue();
                    }
                    valid=1;
                    break;
                case 't':
                case 'T':
                    if (currentGame->player.changeMoves > 0)
                        changePos(currentGame);
                    valid =1;
                    break;
                case 'v':
                case 'V':
                    if (currentGame->won) //Se tiver ganho deve resetar e salvar o ranking
                        saveRanking(currentGame, ranking);
                    return;
                default:
                    printf("\nOpção inválida! Dê uma olhada nas instruções do jogo.\n");
            }
        } while (!valid);
        if (currentGame->won && !wonTold) { //Se ja tiver feito a mensagem e o jogador continuar, então deve-se pular
            printf("\n%s você ganhou!", currentGame->player.name); //Indica ao usuário que ele ganhou!
            do {
                printf("\nVocê deseja continuar jogando? (s/n)\n");
                scanf(" %c", &choice);
                valid=0;
                switch (choice) {
                    case 's':
                    case 'S':
                        wonTold=1;
                        valid=1;
                        break;
                    case 'n':
                    case 'N':
                        saveRanking(currentGame, ranking);
                        return;
                    default:
                        printf("\nOpção inválida!\n");
                        enterContinue();
                        clear();
                }
            } while (!valid);
        }
    }
    clear();
    if (currentGame->won) {
        saveRanking(currentGame, ranking);
    } // Só salva o ranking quando ele decidir sair
}

void newGameMenu(Game *currentGame, LastMove *lastMove) { //Cria o menu de novo jogo
    clear();

    char name[TAM]; //TAM = 27
    printf("Digite seu nome: ");
    getchar();
    fgets(name, TAM, stdin);

    do {
        clear();
        printf("(4) jogo padrão 4 x 4.\n");
        printf("(5) jogo 5 x 5.\n");
        printf("(6) jogo 6 x 6.\n");

        int choice;
        scanf(" %d", &choice);

        switch (choice) { //Caso o usuário digite algo inválido
            case 4:
            case 5:
            case 6:
                newGame(choice, name, currentGame, lastMove);
                return;
            default:
                printf("\nOpção inválida\n");
                enterContinue();
        }
    } while (1);
}

int exitGame(Game *currentGame, LastMove *lastMove) { //Confirmação simples de sair do jogo
    while (1){ //Repete até uma opção válida ser digitada
        clear();
        printf("Você realmente deseja sair? (Sim/Não)\n");

        char choice[3]; //Lê os três caracteres (sim/não)
        scanf("%s", choice);

        switch (choice[0]) { //Só precisa do primeiro caracter para saber se foi sim/não
            //Se for sair não faz nada e volta ao main()
            case 's':
            case 'S':
                reset(currentGame, lastMove);
                return 1;
            case 'n':
            case 'N':
                return 0;
            default:
                printf("\nOpção inválida!\n");
                enterContinue();
        }
    }
}

void start() {
    srand(time(NULL));
    Game currentGame = {.board = NULL}; //Inicia o Game com board NULL
    Ranking ranking[3]; //Inicializa e carrega o ranking (caso exista)
    loadRanking(ranking);
    LastMove lastMove = {0, NULL};
    while (1){
        //Mostra o menu inicial em loop
        clear();

        printf(ANSI_BOLD "Bem Vindo ao " ANSI_ITALIC ANSI_COLOR_YELLOW "2048\n" ANSI_RESET);

        printf("(R) Sair\n");
        printf("(N) Novo Jogo\n");
        printf("(J) Continuar o jogo atual\n");
        printf("(C) Carregar um jogo salvo\n");
        printf("(S) Salvar o jogo atual\n");
        printf("(M) Mostrar Ranking\n");
        printf("(A) Ajuda com as instruções de como jogar\n");

        char choice;
        printf("\nDigite uma opção: ");
        scanf(" %c", &choice);
        switch (choice) {
            case 'R':
            case 'r':
                /*Caso o usuário decida sair ele da return,
                consequentemente voltando para main() e saindo do programa */
                if (exitGame(&currentGame, &lastMove))
                    return;
                break;
            case 'N':
            case 'n': //Entra no menu de novo jogo e roda o jogo
                newGameMenu(&currentGame, &lastMove);
                runGame(&currentGame, ranking, &lastMove);
                break;
            case 'J':
            case 'j':
                if (currentGame.board != NULL) //Se não existir uma board, o jogo não iniciou
                    runGame(&currentGame, ranking, &lastMove);
                else {
                    clear();
                    printf("Nenhum jogo em andamento!");
                    enterContinue();
                }
                break;
            case 'C':
            case 'c':
                if (!loadGame(&currentGame, &lastMove)) //Caso o arquivo não exista a função retorna 1
                    runGame(&currentGame, ranking, &lastMove);
                break;
            case 'S':
            case 's':
                saveGame(currentGame, lastMove);
                break;
            case 'M':
            case 'm':
                showRanking(ranking);
                break;
            case 'A':
            case 'a':
                showHelp();
                break;
            default:
                printf("\nOpção inválida!\n");
                enterContinue();
        }
    }
}

int main() {
    start();
    return 0;
}