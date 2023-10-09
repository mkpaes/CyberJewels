#include "./fileop.h"

#ifndef BOARD_H
#define BOARD_H

typedef struct coord{
    int lin;
    int col;
} coord;

int** initBoard(int mode, int *score, int *level, int *extraPieces);

void freeBoard(int **board);

void getPiece(coord *piece, int lin, int col);

void getDirection(coord piece, int* direction);

void getMove(coord origin, coord *dest, int direction);

void swapPieces(int **board, coord origin, coord dest);

int isValidMove(int **board, coord *origin, coord *dest, int *score, int modLv, int *modCombo);

int popSpecial(int **board, int *score, int *level, int *extraPieces, int *modCombo);

int manageBoard(int **board, int *score, int *level, int *extraPieces, int *modCombo);

int newTrios(int **board, int *score, int *modCombo, int modLv);

void passLevel(int score, int *level, int *extraPieces);

int hasMoves(int **board);

void printBoard(int **board, int score, int level);

#endif /* BOARD_H */