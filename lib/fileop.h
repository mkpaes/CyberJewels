#ifndef __FILEOP__
#define __FILEOP__

void saveScore(int score);

void saveGame(int **board, int score, int level, int extraPieces);

int hasSave();

#endif