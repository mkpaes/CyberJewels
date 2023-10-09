#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileop.h"

#define SIZE 8
#define MAXCHAR 12
#define MAXLINESIZE 100

// ============================== FUNÇÕES INTERNAS ==============================

// Ordena e imprime os 10 melhores no arquivo
void ordenaLeader(FILE *arq){
    char list[11][MAXLINESIZE];
    char line[MAXLINESIZE];
    int posit = 0;

    rewind(arq);

    fgets(line, MAXLINESIZE, arq); // Primeiro nome
    while (!feof(arq) && posit < 11){
        strcpy(list[posit], line);
        fgets(line, MAXLINESIZE, arq);
        posit++;
    }

    // Bubble Sort
    for(int i = 0; i < posit - 1; i++) {
        for(int j = 0; j < posit - 1 - i; j++) {
            if(strcmp(list[j], list[j+1]) < 0){     // Se j vem antes de j+1
                char aux[MAXLINESIZE];
                strcpy(aux, list[j]);               // Copia para aux a palavra em j
                strcpy(list[j], list[j+1]);         // Copia para j a palavra em j + 1
                strcpy(list[j+1], aux);             // Copia para j + 1 a palavra em aux
            }
        }
    }

    if(posit > 10)
        posit = 10;

    arq = fopen("saves/leaderboard.txt", "w");
    for (int i = 0; i < posit; i++) {
        fprintf(arq, "%s", list[i]);
    }
    fclose(arq);
}

// ==============================================================================

// Se a pontuação estiver entre os 10 melhores, ele salva a pontuação
void saveScore(int score){
    FILE *arq;
    char nome[MAXCHAR];
    
    arq = fopen("saves/leaderboard.txt", "a+");

    if(!arq) {
        perror("Não foi possível abrir/criar o arquivo");
        exit(1);
    }

    printf("Insira seu nome: ");
    fgets(nome, MAXCHAR, stdin);
    fprintf(arq, "%06d ", score);
    fprintf(arq, "%s", nome);

    ordenaLeader(arq);
    fclose(arq);
}

// Salva em um arquivo txt o status atual do jogo
void saveGame(int **board, int score, int level, int extraPieces) {
    FILE *arq;
    arq = fopen("saves/save.txt", "w");
    if(!arq) {
        perror("Não foi possivel criar o arquivo");
        exit(1);
    }

    for(int lin = 0; lin < SIZE; lin++){
        for(int col = 0; col < SIZE; col++)
            fprintf(arq, "%d ", board[lin][col]);
        fprintf(arq, "\n");
    }
    fprintf(arq, "%d\n", score);
    fprintf(arq, "%d\n", level);
    fprintf(arq, "%d\n", extraPieces);
    fclose(arq);
}

// Checa se existe um save. Retorna 1 se houver ou 0 caso não.
int hasSave() {
    FILE* arq = fopen ("saves/save.txt", "r");
    if(!arq)
        return 0;
    else 
        return 1;
}