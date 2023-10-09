#ifndef FILEOP_H
#define FILEOP_H

void saveScore(int score);

void saveGame(int **board, int score, int level, int extraPieces);

int hasSave();

#endif /* FILEOP_H */