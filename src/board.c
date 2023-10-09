#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../lib/board.h"

#define SIZE 8
#define UP -1
#define DOWN 1
#define RIGHT 2
#define LEFT -2
#define CENTER_V 3
#define CENTER_H 4

#define RED "\x1B[31m"
#define GRE "\x1B[32m"
#define BLU "\x1B[33m"
#define YEL "\x1B[34m"
#define MAG "\x1B[35m"
#define CYA "\x1b[36m"
#define WHT "\x1B[37m" //"\x1B[0m"

// ============================== FUNÇÕES INTERNAS ==============================

// Escolhe um int aleatório entre 0 e maxValue, retornando esse valor
int randomize(int maxValue){
    return (1 + rand() % maxValue);
}

// Altera a peça em board[lin][col] para outra que seja diferente dos vizinhos
void changePiece(int **board, coord *alvo, int maxValue){
    int flags[maxValue];
    for (int i = 0; i < maxValue; i++)
        flags[i] = 0;

    if (alvo->lin + 1 < SIZE)
        flags[board[alvo->lin + 1][alvo->col] - 1] = 1;
    if (alvo->lin - 1 > 0)
        flags[board[alvo->lin - 1][alvo->col] - 1] = 1;
    if (alvo->col + 1 < SIZE)
        flags[board[alvo->lin][alvo->col + 1] - 1] = 1;
    if (alvo->col - 1 > 0)
        flags[board[alvo->lin][alvo->col - 1] - 1] = 1;

    int i = 0;
    while (flags[i] != 0){
        i++;
    }
    board[alvo->lin][alvo->col] = i + 1;
}

// Preenche a matriz board, utilizando a função randomize
void fillBoard(int **board){
    srand(time(NULL));
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = randomize(5);
}

// Verifica os trios ao redor do alvo, colocando flags no vetor trios para indicar a direção dos trios. 
// Retorna o número de trios encontrados
int hasTrio(int **board, coord *alvo, int *trios){
    // 0 = UP, 1 = DOWN, 2 = RIGHT, 3 = LEFT, 4 = CENTER_V, 5 = CENTER_H
    int flag = 0;

    for(int i = 0; i < 6; i++)
        trios[i] = 0;

    // Trio acima
    if(alvo->lin - 2 >= 0)
        if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin - 1][alvo->col]) % 10)
            if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin - 2][alvo->col]) % 10){
                trios[0] = UP;
                flag++;
            }

    // Trio abaixo:
    if(alvo->lin + 2 < SIZE)
        if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin + 1][alvo->col]) % 10)
            if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin + 2][alvo->col]) % 10) {
                trios[1] = DOWN;
                flag++;
            }
    
    // Trio à direita
    if(alvo->col + 2 < SIZE)
        if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin][alvo->col + 1]) % 10)
            if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin][alvo->col + 2]) % 10){
                trios[2] = RIGHT;
                flag++;
            }

    // Trio à esquerda
    if(alvo->col - 2 >= 0)
        if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin][alvo->col - 1]) % 10)
            if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin][alvo->col - 2]) % 10){
                trios[3] = LEFT;
                flag++;
            }
    
    // Trio no centro vertical
    if(alvo->lin - 1 >= 0 && alvo->lin + 1 < SIZE)
        if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin - 1][alvo->col]) % 10)
            if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin + 1][alvo->col]) % 10){
                trios[4] = CENTER_V;
                flag++;
            }

    // Trio no centro horizontal
    if(alvo->col - 1 >= 0 && alvo->col + 1 < SIZE)
        if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin][alvo->col - 1]) % 10)
            if((abs(board[alvo->lin][alvo->col]) % 10) == abs(board[alvo->lin][alvo->col + 1]) % 10) {
                trios[5] = CENTER_H;
                flag++;
            }

    return flag;
}

// Calcula os pontos feitos
// Mode = 0 soma "value" ajustado pelos modificadores. Mode = 1 soma os "value" trios ajustados pelos modificadores
// Retorna o valor a ser adicionado no score
int setScore(int mode, int value, int modLv, int *modCombo){
    int modifs = 10 * modLv * (*modCombo);
    int score = 0;

    if (mode == 0) {
        (*modCombo)++; 
        return (value * modifs);
    } else {
        for (int i = 0; i < value; i++){
            score += 10 * modLv * (*modCombo);
            (*modCombo)++;
        }
        return score;
    }
}

// Marca o pivot de um movimento
void markPivot(int **board, coord *alvo, int *trios){
    // Trio normal
    for(int i = 0; i < 6; i++)
        if(trios[i] != 0){
            board[alvo->lin][alvo->col] = - abs(board[alvo->lin][alvo->col]);
            break;
        }

    // Combo de 4 peças juntas em uma linha (valor 10)
    if((trios[4] == CENTER_V) && (trios[0] == UP || trios[1] == DOWN))
        board[alvo->lin][alvo->col] = 10 + (abs(board[alvo->lin][alvo->col]) % 10);
    if((trios[5] == CENTER_H) && (trios[2] == RIGHT || trios[3] == LEFT))
        board[alvo->lin][alvo->col] = 10 + (abs(board[alvo->lin][alvo->col]) % 10);

    // Combo no formato T (valor 20)
    if((trios[0] == UP || trios[1] == DOWN) && (trios[2] == RIGHT || trios[3] == LEFT))
        board[alvo->lin][alvo->col] = 20 + (abs(board[alvo->lin][alvo->col]) % 10);
    if((trios[4] == CENTER_V) && (trios[2] == RIGHT || trios[3] == LEFT))
        board[alvo->lin][alvo->col] = 20 + (abs(board[alvo->lin][alvo->col]) % 10);
    if((trios[5] == CENTER_H) && (trios[0] == UP || trios[1] == DOWN))
        board[alvo->lin][alvo->col] = 20 + (abs(board[alvo->lin][alvo->col]) % 10);

    // Combo de 5 peças juntas
    if(trios[0] == UP && trios[1] == DOWN)      // Em cima e embaixo
        board[alvo->lin][alvo->col] = 9;
    if(trios[2] == RIGHT && trios[3] == LEFT)   // Direita e esquerda
        board[alvo->lin][alvo->col] = 9;
}

// Troca o valor dos trios pelo seu valor negativo
void markPieces(int **board, coord *alvo, int *trios){
    // 0 = UP, 1 = DOWN, 2 = RIGHT, 3 = LEFT, 4 = CENTER_V, 5 = CENTER_H
    if(trios[0] == UP) {
        board[alvo->lin + UP][alvo->col] = - abs(board[alvo->lin + UP][alvo->col]);
        board[alvo->lin + (2 * UP)][alvo->col] = - abs(board[alvo->lin + (2 * UP)][alvo->col]);
    }

    if(trios[1] == DOWN) {
        board[alvo->lin + DOWN][alvo->col] = - abs(board[alvo->lin + DOWN][alvo->col]);
        board[alvo->lin + (2 * DOWN)][alvo->col] = - abs(board[alvo->lin + (2 * DOWN)][alvo->col]);
    }

    if (trios[2] == RIGHT) {
        board[alvo->lin][alvo->col + (RIGHT/2)] = - abs(board[alvo->lin][alvo->col + (RIGHT/2)]);
        board[alvo->lin][alvo->col + RIGHT] = - abs(board[alvo->lin][alvo->col + RIGHT]);
    } 

    if(trios[3] == LEFT){
        board[alvo->lin][alvo->col + (LEFT/2)] = - abs(board[alvo->lin][alvo->col + (LEFT/2)]);
        board[alvo->lin][alvo->col + LEFT] = - abs(board[alvo->lin][alvo->col + LEFT]);
    } 
    
    if (trios[4] == CENTER_V) {
        board[alvo->lin - 1][alvo->col] = - abs(board[alvo->lin - 1][alvo->col]);
        board[alvo->lin + 1][alvo->col] = - abs(board[alvo->lin + 1][alvo->col]);
    } 
    
    if (trios[5] == CENTER_H) {
        board[alvo->lin][alvo->col - 1] = - abs(board[alvo->lin][alvo->col - 1]);
        board[alvo->lin][alvo->col + 1] = - abs(board[alvo->lin][alvo->col + 1]);
    }

    markPivot(board, alvo, trios);
}

// Troca as peças afetadas pela peça especial, retorna o número de peças estouradas
int markPiecesSpecial(int **board, int lin, int col) {
    if(board[lin][col] < -20) { // Explosão em cruz
        for(int i = 0; i < SIZE; i++) {
            board[i][col] = - abs(board[i][col]);
            board[lin][i] = - abs(board[lin][i]);
        }
        return (2 * SIZE);
    } else { // Explosão em volta
        if(lin - 1 >= 0)   // Muda a peça acima
            board[lin - 1][col] = - abs(board[lin - 1][col]);
        if(lin + 1 < SIZE) // Muda a peça abaixo
            board[lin + 1][col] = - abs(board[lin + 1][col]); 
        if(col + 1 < SIZE) // Muda a peça à direita
            board[lin][col + 1] = - abs(board[lin][col + 1]);
        if(col - 1 >= 0)   // Muda a peça à esquerda
            board[lin][col - 1] = - abs(board[lin][col - 1]);
        if(lin - 1 >= 0 && col - 1 >= 0)   // Muda diagonal superior direita
            board[lin - 1][col - 1] = - abs(board[lin - 1][col - 1]);
        if(lin - 1 >= 0 && col + 1 < SIZE) // Muda diagonal superior esquerda
            board[lin - 1][col + 1] = - abs(board[lin - 1][col + 1]);
        if(lin + 1 < SIZE && col - 1 >= 0) // Muda diagonal inferior direita
            board[lin + 1][col - 1] = - abs(board[lin + 1][col - 1]);
        if(lin + 1 < SIZE && col + 1 < SIZE) // Muda diagonal inferior esquerda
            board[lin + 1][col + 1] = - abs(board[lin + 1][col + 1]);
        return 9;
    }
}

// Auxiliar para evitar duplicação da peça especial tipo O. Ela verifica os trios da próxima peça na direção dir.
// Retorna o número de trios.
int verifyNext(int **board, coord alvo, int dir){
    int numTrios, trios[6];
    coord aux;

    if(dir == RIGHT) {
        aux.lin = alvo.lin;
        aux.col = alvo.col + 1;
    }

    if(dir == DOWN) {
        aux.lin = alvo.lin + 1;
        aux.col = alvo.col;
    }

    numTrios = hasTrio(board, &aux, trios);
    markPivot(board, &aux, trios);
    if(board[aux.lin][aux.col] > 10 && board[aux.lin][aux.col] < 20)
        board[aux.lin][aux.col] = - abs(board[aux.lin][aux.col] % 10);

    //printBoard(board, 1, 1);
    return numTrios;
}

// Checa se existe trio no board. Retorna o número de trios encontrados. 
// Se mode = 0, troca as peças. Se mode 1, marca elas.
int checkTrios(int **board, int mode){
    int numTrios = 0, totalTrios = 0, trios[6];
    coord alvo;

    // Loop para checar 1 a 1 se possui trios
    for (int lin = 0; lin < SIZE; lin ++){   
        for (int col = 0; col < SIZE; col++){
            alvo.lin = lin; alvo.col = col;
            if(board[lin][col] < 0 && mode)
                col++;
            else {
                numTrios = hasTrio(board, &alvo, trios);
                if(numTrios) {
                    if(mode == 0)
                        changePiece(board, &alvo, 5);
                    else
                        markPivot(board, &alvo, trios);
                }
                totalTrios += numTrios;

                // Evita duplicar a peça especial tipo O na horizontal
                if(trios[5] == CENTER_H && trios[2] == RIGHT && mode) {
                    totalTrios += verifyNext(board, alvo, RIGHT);
                    col++;
                    // Caso a peça pulada seja uma coringa
                    if(board[lin][col] == 9) {   
                        board[lin][col - 1] = - abs(board[lin][col - 1] % 10);
                        board[lin][col + 1] = - abs(board[lin][col + 1]);
                        board[lin][col + 2] = - abs(board[lin][col + 2]);
                        totalTrios += 3;
                    }
                } 

                // Evita duplicar a peça especial tipo O na vertical
                else if(trios[4] == CENTER_V && trios[1] == DOWN && mode) {
                    totalTrios += verifyNext(board, alvo, DOWN);
                    // Caso a peça pulada seja uma coringa
                    if(board[lin + 1][col] == 9) {   
                        board[lin][col] = - abs(board[lin][col] % 10);
                        board[lin + 2][col] = - abs(board[lin + 2][col]);
                        board[lin + 3][col] = - abs(board[lin + 3][col]);
                        totalTrios += 3;
                    }

                }
            }
        }
    }

    return (totalTrios/3);
}

// Faz a coluna cair
void dropColumn(int **board, int lin, int col, int extraPieces){
    for(int currentLine = lin; currentLine >= 0; currentLine--){
        if(currentLine - 1 >= 0)
            board[currentLine][col] = board[currentLine - 1][col];
        else
            board[currentLine][col] = randomize(5 + extraPieces);
    }
}

// Se o score for suficiente, passa de level
void passLevel(int score, int *level, int *extraPieces){
    int scoreNeeded = 800;

    for(int i = 1; i < (*level); i++)
        scoreNeeded += 1000 * i;
    if(score > scoreNeeded)
        (*level)++;
    if((*level) == 3)
        (*extraPieces) = 1;
    if((*level) == 9)
        (*extraPieces) = 2;
}

// Se existe uma combinação tipo A, retorna 1, caso o contrário retorna 0
int hasMovesTypeA(int **board, coord *alvo){
    // Padrão de combinação A (x é avaliado): 
    //              o | x | o
    //              x | o | x
    //              o | x | o

    // Checagem horizontal
    if((alvo->col < SIZE - 2) && (board[alvo->lin][alvo->col] == board[alvo->lin][alvo->col+2])){
        // Existe uma linha acima com uma peça adequada
        if((alvo->lin - 1 >= 0) && (board[alvo->lin][alvo->col] == board[alvo->lin - 1][alvo->col + 1]))
            return 1;
        // Existe uma linha abaixo com uma peça adequada
        if((alvo->lin + 1 < SIZE) && (board[alvo->lin][alvo->col] == board[alvo->lin + 1][alvo->col + 1]))
            return 1;
    }
    // Checagem Vertical
    if((alvo->lin < SIZE - 2) && (board[alvo->lin][alvo->col] == board[alvo->lin + 2][alvo->col])){
        // Existe uma coluna anterior com uma peça adequada
        if((alvo->col - 1 >= 0) && (board[alvo->lin][alvo->col] == board[alvo->lin + 1][alvo->col - 1]))
            return 1;
        // Existe uma coluna seguinte com uma peça adequada
        if((alvo->col + 1 < SIZE) && (board[alvo->lin][alvo->col] == board[alvo->lin + 1][alvo->col + 1]))
            return 1;
    }
    return 0;
}

// Se existe uma combinação tipo B, retorna 1, caso o contrário retorna 0
int hasMovesTypeB(int **board, coord *alvo){
    // Padrão de combinação B (x é avaliado):
    //        o | x | o | o | x | o
    //        x | o | x | x | o | x
    //        o | x | o | o | x | o

    // Verificação Horizontal
    if(alvo->col < SIZE - 2) {
        if (board[alvo->lin][alvo->col] == board[alvo->lin][alvo->col + 1]) { // Encontrou uma dupla
            // Existe uma linha acima com uma peça adequada
            if(alvo->lin - 1 >= 0){ 
                if(board[alvo->lin][alvo->col] == board[alvo->lin - 1][alvo->col + 2])
                    return 1;
                if((alvo->col - 1 >= 0) && (board[alvo->lin][alvo->col] == board[alvo->lin - 1][alvo->col - 1]))
                    return 1;
            }
            // Existe uma linha abaixo com uma peça adequada
            if(alvo->lin + 1 < SIZE){
                if(board[alvo->lin][alvo->col] == board[alvo->lin + 1][alvo->col + 2])
                    return 1;
                if((alvo->col - 1 >= 0) && (board[alvo->lin][alvo->col] == board[alvo->lin + 1][alvo->col - 1]))
                    return 1;
            }
            // Existe uma peça adequada na mesma linha
            if(alvo->col + 3 < SIZE && board[alvo->lin][alvo->col] == board[alvo->lin][alvo->col + 3])
                return 1;
            if(alvo->col - 2 >= 0 && board[alvo->lin][alvo->col] == board[alvo->lin][alvo->col - 2])
                return 1;
        } 
    }
    // Verificação Vertical
    if(alvo->lin < SIZE - 2) {
        if (board[alvo->lin][alvo->col] == board[alvo->lin + 1][alvo->col]) { // Encontrou uma dupla
            // Existe uma linha à esquerda com uma peça adequada
            if(alvo->col - 1 >= 0){ 
                if(board[alvo->lin][alvo->col] == board[alvo->lin + 2][alvo->col - 1])
                    return 1;
                if((alvo->lin - 1 >= 0) && (board[alvo->lin][alvo->col] == board[alvo->lin - 1][alvo->col - 1]))
                    return 1;
            }
            // Existe uma linha à direita com uma peça adequada
            if(alvo->col + 1 < SIZE){
                if(board[alvo->lin][alvo->col] == board[alvo->lin + 2][alvo->col + 1])
                    return 1;
                if((alvo->lin - 1 >= 0) && (board[alvo->lin][alvo->col] == board[alvo->lin - 1][alvo->col + 1]))
                    return 1;
            }
            // Existe uma peça adequada na mesma coluna
            if(alvo->lin + 3 < SIZE && board[alvo->lin][alvo->col] == board[alvo->lin + 3][alvo->col])
                return 1;
            if(alvo->lin - 2 >= 0 && board[alvo->lin][alvo->col] == board[alvo->lin - 2][alvo->col])
                return 1;
        } 
    }

    return 0;
}

// ==============================================================================

// Inicia a matriz (aloca, preence e valida)
int **initBoard(int mode, int *score, int *level, int *extraPieces){
    int **board;

    // Alocação do tabuleiro
    board = malloc(SIZE * sizeof(int *));
    for (int i = 0; i < SIZE; i++)
        board[i] = malloc(SIZE * sizeof(int));

    // Novo jogo
    if(mode == 1) {
        fillBoard(board);
        checkTrios(board, 0);
        (*score) = 0;
        (*level) = 1;
        (*extraPieces) = 0;
        saveGame(board, *score, *level, *extraPieces);
    } 
    // Continuar jogo
    else { 
        FILE *arq;
        arq = fopen("saves/save.txt", "r");
        if(!arq){
            perror("Não foi possível abrir o arquivo");
            exit(1);
        }
        // Leitura do tabuleiro
        for (int lin = 0; lin < SIZE; lin++)
            for (int col = 0; col < SIZE; col++){
                fscanf(arq, "%d", &board[lin][col]);
            }
        // Leitura do score e level
        fscanf(arq, "%d", score);
        fscanf(arq, "%d", level);
        fscanf(arq, "%d", extraPieces);
        fclose(arq);
    }
    //printBoard(board, *score, *level);
    return board;
}

// Libera a memória da Matriz
void freeBoard(int **board){
    for(int i = 0; i < SIZE; i++)
        free(board[i]);
    free(board);
}

// Obtém a jogada a ser realizada
void getPiece(coord *piece, int lin, int col){
    // Coordenadas da peça
    piece->lin = lin;
    piece->col = col;
}

// Troca de lugar a peça origin com a peça dest
void swapPieces(int **board, coord origin, coord dest){
    int aux = board[origin.lin][origin.col];                        // aux recebe o valor em origin
    board[origin.lin][origin.col] = board[dest.lin][dest.col];      // origin recebe o valor em dest
    board[dest.lin][dest.col] = aux;                                // dest recebe o valor em aux
}

// Checa se existe trios tanto para o origin quanto para o dest. Retorna 1 se houver trio e 0 caso contrário.
int isValidMove(int **board, coord *origin, coord *dest, int *score, int modLv, int *modCombo){
    int numTrios = 0, totalTrios = 0, typePiece = 0, specialPop = 0;
    int trios[6] = {0,0,0,0,0,0};

    // Verifica se há um coringa
    if(board[origin->lin][origin->col] == 9){
        typePiece = (board[dest->lin][dest->col] % 10);     // Pega o valor com qual trocou
        board[origin->lin][origin->col] = -9;
    } else if(board[dest->lin][dest->col] == 9){
        typePiece = (board[origin->lin][origin->col] % 10); // Pega o valor com qual trocou
        board[dest->lin][dest->col] = -9;
    }

    // Se houver um coringa
    if(typePiece){
        for(int i = 0; i < SIZE; i++){
            for(int j = 0; j < SIZE; j++){
                if((board[i][j] % 10) == typePiece){
                    board[i][j] = - abs(board[i][j]);
                    specialPop++;
                }
            }
        }
        // Atualiza o score
        (*score) += setScore(0, specialPop, modLv, modCombo);
        return 1;
    } else {
        // Checa trios para a casa de origem
        numTrios = hasTrio(board, origin, trios);
        totalTrios += numTrios;
        if(numTrios) 
            markPieces(board, origin, trios);

        // Checa trios para a casa de destino
        numTrios = hasTrio(board, dest, trios);
        totalTrios += numTrios;
        if(numTrios)
            markPieces(board, dest, trios);

        // Atualiza o score
        if(totalTrios) {
            (*score) += setScore(1, totalTrios, modLv, modCombo);
            return 1;
        }
    }

    return 0;
}

// Se encontrar uma peça especial estourada, aplica seu efeito, estourando novas peças
int popSpecial (int **board, int *score, int *level, int *extraPieces, int *modCombo){
    int scoreAdd = 0, flag = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) 
            if (board[i][j] < -10) { 
                scoreAdd = markPiecesSpecial(board, i, j);
                (*score) += setScore(0, scoreAdd, *level, modCombo);
                //printBoard(board, *score, *level);
                board[i][j] = - abs(board[i][j] % 10);
                flag++;
            }
    passLevel(*score, level, extraPieces);
    return(flag);
}

// Remove todas as flags de explosão e dropa as colunas em 1
int manageBoard (int **board, int *score, int *level, int *extraPieces, int *modCombo){
    int flag = 0;
    // Loop para estourar as peças
    for (int col = 0; col < SIZE; col++) {
        for (int lin = SIZE - 1; lin >= 0; lin--){
            if (board[lin][col] < 0){
                dropColumn(board, lin, col, *extraPieces);
                flag++;
                break;
            }
        }
    }
    //printBoard(board, *score, *level);
    return flag;
}

// Checa se existem novos trios, retorna o número de trios
int newTrios(int **board, int *score, int *modCombo, int modLv){
    int numTrios = 0;
    numTrios = checkTrios(board, 1);
    (*score) += setScore(1, numTrios, modLv, modCombo);
    return numTrios;
}

// Checa se existe possibilidade de formar trios, retorna 1 se existir ou 0 caso contrário
int hasMoves(int **board){
    coord alvo;
    for(int i = 0; i < SIZE; i++)
        for(int j = 0; j < SIZE; j++){
            alvo.lin = i; alvo.col = j;
            if (hasMovesTypeA(board, &alvo))
                return 1;
            if (hasMovesTypeB(board, &alvo))
                return 1;
        }
    return 0;
}

// Imprime o mapa
void printBoard(int **board, int score, int level){
    char *color;
    // Cabeçalho
    printf ("\n  ");
    for (int i = 0; i < SIZE; i++)
        printf(" %2.1d|", i);
    printf("\n");

    // Board
    for (int i = 0; i < SIZE; i++) {
        printf(WHT "%d ", i);
        for (int j = 0; j < SIZE; j++) {
            if(board[i][j] % 10 == 1) {
                color = RED;
            } else if (board[i][j] % 10 == 2){
                color = GRE;
            } else if (board[i][j] % 10 == 3){
                color = BLU;
            } else if (board[i][j] % 10 == 4){
                color = YEL;
            } else if (board[i][j] % 10 == 5){
                color = MAG;
            } else if (board[i][j] % 10 == 6){
                color = CYA;
            } else {
                color = WHT;
            }
            printf("%s %2.1i ", color, board[i][j]);
        }
        printf("\n");
    }
    printf("%sScore: %d\nLevel: %d\n", WHT, score, level);
}