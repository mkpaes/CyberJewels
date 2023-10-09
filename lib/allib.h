#ifndef __ALLIB__
#define __ALLIB__

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "board.h"

typedef struct TYPE_BTN{
    int x0, y0;
    int width, height;
    int state;
} TYPE_BTN;

typedef struct TYPE_STATE {
    int state;
    int inState;
    int score;
    int level;
    int extraPieces;
} TYPE_STATE;

// ============================= FUNÇÕES P/ ALLEGRO =============================
void must_init(bool test, const char *description);

void initAllegro();

ALLEGRO_DISPLAY *setDisplay(int x, int y);

void initAudio(ALLEGRO_AUDIO_STREAM* musicTheme, float gain);

void registerEvents(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_DISPLAY* disp, ALLEGRO_TIMER* timer);

ALLEGRO_BITMAP **initSprites();

TYPE_BTN **initMenuBtn();

void freeMenuBtns(TYPE_BTN **btnMenu);

void deinitSprites(ALLEGRO_BITMAP **icons);

void deinitAudios(ALLEGRO_AUDIO_STREAM *songTheme, ALLEGRO_AUDIO_STREAM *songGame, ALLEGRO_SAMPLE *soundSelect,
ALLEGRO_SAMPLE *soundConfirm, ALLEGRO_SAMPLE *soundKonami);

// ============================= FUNÇÕES AUXILIARES =============================

int mouseClicked(int mouseBtn, int isPressed);

int isOverBtn(TYPE_BTN *btn, int mouseX, int mouseY, ALLEGRO_SAMPLE *soundSelect);

int updateBtn(TYPE_BTN *btn, int event);

int isInBoard(int mouseX, int mouseY);

int validSwap(int oriLin, int oriCol, int destLin, int destCol);

void getPiecePos(int mouseX, int mouseY, int *px, int *py);

void drawSelected(int px, int py);

void drawScreen(TYPE_STATE gS, TYPE_BTN **btn, int **board, coord origin, coord dest, int swap, float dropA, ALLEGRO_BITMAP **icons);

#endif