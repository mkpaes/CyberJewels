#include <stdio.h>
#include "../lib/allib.h"

#define STATE_QUIT     -1
#define STATE_MENU      0
#define STATE_LEADER    1
#define STATE_GAME      2
#define STATE_HELP      3
#define STATE_END       4

#define NEW_GAME 0
#define CONTINUE 1
#define LEADER   2
#define QUIT     3

#define PLAY    0
#define SWAP    1
#define DESWAP  2
#define RESOLVE 3
#define DROP    4
#define VERIFY  5

#define BTN_NEUTRAL 0
#define BTN_MOUSE   1
#define BTN_CLICKED 2

#define RELEASE 0
#define HOLD 1

#define BOARDX0 390
#define BOARDX1 1010
#define BOARDY0 50
#define BOARDY1 680
#define NUM_ICONS 22

#define SIZE  8
#define UP   -1
#define DOWN  1
#define LEFT -2
#define RIGHT 2

#define VOLUME 0.5

// ============================== FUNÇÕES INTERNAS ==============================

// Cria um botão com coordenadas iniciais x e y, com largura width e altura height, retornando um ponteiro
TYPE_BTN* createBtn(int x0, int y0, int width, int height){
    TYPE_BTN *btn = malloc(sizeof(TYPE_BTN));
    btn->x0 = x0;
    btn->y0 = y0;
    btn->width = width;
    btn->height = height;
    btn->state = BTN_NEUTRAL;
    return(btn);
}

// Desenha a tela de Menu
void drawMenu (TYPE_BTN **btnMenu) {
    ALLEGRO_BITMAP *menu;
    if(btnMenu[NEW_GAME]->state != BTN_NEUTRAL)
        menu = al_load_bitmap("resources/images/menuBG01.png");
    else if(btnMenu[CONTINUE]->state != BTN_NEUTRAL)
        menu = al_load_bitmap("resources/images/menuBG02.png");
    else if(btnMenu[LEADER]->state != BTN_NEUTRAL)
        menu = al_load_bitmap("resources/images/menuBG03.png");
    else if(btnMenu[QUIT]->state != BTN_NEUTRAL)
        menu = al_load_bitmap("resources/images/menuBG04.png");
    else
        menu = al_load_bitmap("resources/images/menuBG00.png");

    must_init(menu, "menu");
    al_draw_bitmap(menu, 0, 0, 0);
    al_destroy_bitmap(menu);
}

// Desenha a tela de Leaderboard
void drawLeader() {
    // Background
    ALLEGRO_BITMAP *leader = al_load_bitmap("resources/images/leaderBG.png");
    must_init(leader, "leader");
    al_draw_bitmap(leader, 0, 0, 0);
    al_destroy_bitmap(leader);

    // Conteúdo 
    ALLEGRO_FONT *font = al_load_ttf_font("resources/fonts/airstrike.ttf", 40, 0);
    must_init(font, "font");

    FILE *arq = fopen("saves/leaderboard.txt", "a+");
    if(!arq){
        perror("Não foi possível abrir o arquivo leaderboard");
        exit(1);
    }
    rewind(arq);

    int desX = 600, desY = 180, numLin = 0;
    char line[16]; int score;
    fscanf(arq, "%d", &score);
    fscanf(arq, "%[^\n]", line);
    while (!feof(arq)) {
        al_draw_textf(font, al_map_rgb(247,221,87), desX - 100, desY + (numLin * 46), ALLEGRO_ALIGN_CENTER, "%06d", score);
        al_draw_textf(font, al_map_rgb(216,240,242), desX, desY + (numLin * 46), ALLEGRO_ALIGN_LEFT, "%s", line);
        fscanf(arq, "%06d", &score);
        fscanf(arq, "%[^\n]", line);
        numLin++;
    }
    al_destroy_font(font);

    font = al_load_ttf_font("resources/fonts/airstrike.ttf", 20, 0);
    must_init(font, "font");
    al_draw_textf(font, al_map_rgb(255, 255, 255), desX+30, 690, ALLEGRO_ALIGN_CENTER, "<Pressione qualquer tecla para voltar>");
    al_destroy_font(font);

    fclose(arq);
}

// Desenha a animação de drop de coluna
void drawDrop(int **board, int lin, int col, float dropA, ALLEGRO_BITMAP **icons){
    int dx = 380, dy = 40, tam = 80, pixels = tam * (dropA/60);
    for(int currentLine = lin - 1; currentLine >= 0; currentLine--){
        if(board[currentLine][col] < 0)
            continue;
        else if(board[currentLine][col] == 9) {   // Peça coringa
            al_draw_bitmap(icons[0], dx + (tam * col), dy + (tam * currentLine) + pixels, 0);
        } else if(board[currentLine][col] < 10) {   // Peça comum
            al_draw_bitmap(icons[(board[currentLine][col])], dx + (tam * col), dy + (tam * currentLine) + pixels, 0);
        } else if(board[currentLine][col] < 20) {   // Peça especial Tipo O
            al_draw_bitmap(icons[(board[currentLine][col]) - 3], dx + (tam * col), dy + (tam * currentLine) + pixels, 0);
        } else {                            // Peça especial Tipo T
            al_draw_bitmap(icons[(board[currentLine][col]) - 6], dx + (tam * col), dy + (tam * currentLine) + pixels, 0);
        }
    }
}

// Desenha a tela de Game, flag indica para NÃO desenhar origin e dest
void drawGame(int **board, coord origin, coord dest, int level, int score, float dropA, ALLEGRO_BITMAP **icons, int flag){
    // Desenha o BG
    ALLEGRO_BITMAP *gamebg = al_load_bitmap("resources/images/gameBG.png");
    al_draw_bitmap(gamebg, 0, 0, 0);
    al_destroy_bitmap(gamebg);

    // Desenha o score
    ALLEGRO_FONT* font = al_load_ttf_font("resources/fonts/Technos-PKDZP.ttf", 40, 0);
    must_init(font, "font");
    al_draw_textf(font, al_map_rgb(255, 255, 255), 172, 150, ALLEGRO_ALIGN_CENTRE, "%06d", score);
    al_destroy_font(font);

    font = al_load_ttf_font("resources/fonts/airstrike.ttf", 15, 0);
    must_init(font, "font");
    al_draw_textf(font, al_map_rgb(255, 255, 255), 172, 220, ALLEGRO_ALIGN_CENTRE, "Level : %d", level);
    al_destroy_font(font);

    // Desenha o board
    int dx = 380, dy = 40, tam = 80;
    for(int col = 0; col < SIZE; col++) {
        int linAux = SIZE - 1;

        // Encontra o primeiro negativo da coluna
        while(linAux >= 0 && board[linAux][col] > 0)
           linAux--;

        if(linAux > 0)
            drawDrop(board, linAux, col, dropA, icons);

        for(int lin = linAux + 1; lin < SIZE; lin++){
            if(((lin == origin.lin && col == origin.col) || (lin == dest.lin && col == dest.col)) && flag){
                continue;                       // Peças selecionadas serão tratadas individualmente
            } else if(board[lin][col] < 0) {    // Peças estouradas
                continue;
            } else if(board[lin][col] == 9) {   // Peça coringa
                al_draw_bitmap(icons[0], dx + (tam * col), dy + (tam * lin), 0);
            } else if(board[lin][col] < 10) {   // Peça comum
                al_draw_bitmap(icons[(board[lin][col])], dx + (tam * col), dy + (tam * lin), 0);
            } else if(board[lin][col] < 20) {   // Peça especial Tipo O
                al_draw_bitmap(icons[(board[lin][col]) - 3], dx + (tam * col), dy + (tam * lin), 0);
            } else {                            // Peça especial Tipo T
                al_draw_bitmap(icons[(board[lin][col]) - 6], dx + (tam * col), dy + (tam * lin), 0);
            }
        }
    }
}

// Desenha a animação de troca
void drawSwap(int **board, coord alvo, int swap, int dir, ALLEGRO_BITMAP **icons){
    int dx = 380, dy = 40, tam = 80;
    int icon;

    // Descobre o icone do alvo
    if(board[alvo.lin][alvo.col] == 9) {        // Peça coringa
        icon = 0;
    } else if(board[alvo.lin][alvo.col] > 20 ) {// Peça especial Tipo T
        icon = (board[alvo.lin][alvo.col]) - 6;
    } else if(board[alvo.lin][alvo.col] > 10) { // Peça especial Tipo O
        icon = board[alvo.lin][alvo.col] - 3;
    } else {                                    // Peça comum
        icon = board[alvo.lin][alvo.col];
    }

    // Desenha o alvo na tela
    switch (dir) {
    case UP:
        al_draw_bitmap(icons[icon], dx + (tam * alvo.col), dy + (tam * alvo.lin) - swap, 0);
        break;
    case DOWN:
        al_draw_bitmap(icons[icon], dx + (tam * alvo.col), dy + (tam * alvo.lin) + swap, 0);
        break;
    case RIGHT:
        al_draw_bitmap(icons[icon], dx + (tam * alvo.col) + swap, dy + (tam * alvo.lin), 0);
        break;
    case LEFT:
        al_draw_bitmap(icons[icon], dx + (tam * alvo.col) - swap, dy + (tam * alvo.lin), 0);
        break;
    }
}

// Faz a lógica da troca
void doSwap(int **board, coord origin, coord dest, int swap, ALLEGRO_BITMAP **icons){
    if(origin.lin == dest.lin){
        if(dest.col > origin.col) { // Origem indo para direita
            drawSwap(board, origin, swap, RIGHT, icons);
            drawSwap(board, dest, swap, LEFT, icons);
        } else {                    // Origem indo para esquerda
            drawSwap(board, origin, swap, LEFT, icons);
            drawSwap(board, dest, swap, RIGHT, icons);
        }
    } else if (origin.col == dest.col) {
        if(dest.lin > origin.lin) { // Origem indo para baixo
            drawSwap(board, origin, swap, DOWN, icons);
            drawSwap(board, dest, swap, UP, icons);
        } else {                    // Origem indo para cima
            drawSwap(board, origin, swap, UP, icons);
            drawSwap(board, dest, swap, DOWN, icons);
        }

    }
}

// Desenha a tela de ajuda/créditos
void drawHelp(){
    ALLEGRO_BITMAP *help = al_load_bitmap("resources/images/helpBG.png");
    must_init(help, "help");
    al_draw_bitmap(help, 0, 0, 0);
    al_destroy_bitmap(help);
}

// ============================= FUNÇÕES P/ ALLEGRO =============================

// Testa se foi possível iniciar o alvo, imprimindo o erro caso não consiga
void must_init(bool test, const char *description) {
    if(test) return;

    printf("Não foi possível inicializar alvo: %s\n", description);
    exit(1);
}

// Inicia algumas partes essenciais para o allegro
void initAllegro(){
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");
    must_init(al_init_image_addon(), "image addon");
    must_init(al_init_font_addon(), "font");
    must_init(al_init_ttf_addon(), "ttf");
    must_init(al_init_primitives_addon(), "primitives"); 
    must_init(al_install_audio(), "audio");
    must_init(al_init_acodec_addon(), "audio codecs");
    must_init(al_reserve_samples(16), "reserve samples"); 
}

// Configura o display e ajusta o tamanho para X_pixels x Y_pixels
ALLEGRO_DISPLAY *setDisplay(int x_pixels, int y_pixels){
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    return al_create_display(x_pixels, y_pixels);
}

// Seta as configurações da musica
void initAudio(ALLEGRO_AUDIO_STREAM* musicTheme, float gain){
    al_set_audio_stream_gain(musicTheme, gain);
    al_set_audio_stream_playmode(musicTheme, ALLEGRO_PLAYMODE_LOOP);
}

// Registra eventos essenciais na queue
void registerEvents(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_DISPLAY* disp, ALLEGRO_TIMER* timer){
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
}

// Carrega os sprites do jogo em um vetor de bitmaps e então o retorna
ALLEGRO_BITMAP **initSprites() {
    ALLEGRO_BITMAP **icons = malloc((NUM_ICONS * sizeof(ALLEGRO_BITMAP*))); // 7 tipos com 3 variações + especial
    icons[0] = al_load_bitmap("resources/icons/icon0.png");
    for(int j = 0; j < 3; j++) {        // Cada variação
        for(int i = 1; i <= 7; i++){    // Cada peça
            char *ext = ".png"; 
            char path[40] = "resources/icons/icon";
            path[20] = (char) i + 64;
            path[21] = (char) j + 49;
            path[22] = '\0';
            icons[(7 * j) + i] = al_load_bitmap(strcat(path, ext));
        }
    }
    return(icons);
} 

// Desaloca os sprites
void deinitSprites(ALLEGRO_BITMAP **icons) {
    for(int i = 0; i < NUM_ICONS; i++)
        al_destroy_bitmap(icons[i]);
    free(icons);
}

// Inicia os botões do menu
TYPE_BTN **initMenuBtn(){
    int btnWidth = 439; int btnHeight = 78; 
    TYPE_BTN **btn = malloc(4 * sizeof(TYPE_BTN*));
    btn[NEW_GAME] = createBtn(395, 372, btnWidth, btnHeight);
    btn[CONTINUE] = createBtn(395, 458, btnWidth, btnHeight);
    btn[LEADER] = createBtn(395, 544, btnWidth, btnHeight);
    btn[QUIT] = createBtn(395, 629, btnWidth, btnHeight);
    return(btn);
}

// Libera memória dos botões do menu
void freeMenuBtns(TYPE_BTN **btnMenu){
    for (int i = 0; i < 4; i++){
        free(btnMenu[i]);   // Libera os botões
    }
    free(btnMenu);  // Libera o ponteiro para os botões
}

// Desaloca os audios
void deinitAudios(ALLEGRO_AUDIO_STREAM *songTheme, ALLEGRO_AUDIO_STREAM *songGame, ALLEGRO_SAMPLE *soundSelect,
ALLEGRO_SAMPLE *soundConfirm, ALLEGRO_SAMPLE *soundKonami) {
    al_destroy_audio_stream(songTheme);
    al_destroy_audio_stream(songGame);
    al_destroy_sample(soundSelect);
    al_destroy_sample(soundConfirm);
    al_destroy_sample(soundKonami);
}

// ============================= FUNÇÕES AUXILIARES =============================

// Identifica a mudança de estado do botão do mouse. Retorna 1 se foi pressionado, 0 se foi solto e -1 caso contrário
int mouseClicked(int mouseBtn, int isPressed){
    if(mouseBtn == RELEASE && isPressed)
        return 1;

    if (mouseBtn == HOLD && !isPressed)
        return 0;

    return -1;
}

// Checa se o mouse está em cima do botão. Retorna 1 caso esteja e 0 caso contrário
int isOverBtn(TYPE_BTN *btn, int mouseX, int mouseY, ALLEGRO_SAMPLE *soundSelect){
    int x0 = btn->x0;
    int x1 = btn->x0 + btn->width;
    int y0 = btn->y0;
    int y1 = btn->y0 + btn->height;

    if((mouseX > x0 && mouseX < x1) && (mouseY > y0 && mouseY < y1)) { 
        if(btn->state == BTN_NEUTRAL){
            al_play_sample(soundSelect, VOLUME, 0.0, 4.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            btn->state = BTN_MOUSE;
        }
        return 1;
    } 

    if(btn->state != BTN_CLICKED)
        btn->state = BTN_NEUTRAL;

    return 0;
}

// Atualiza o estado do botão e retorna 1 se ele foi ativado
int updateBtn(TYPE_BTN *btn, int event) {
    if(btn->state == BTN_MOUSE && event == HOLD) {
        btn->state = BTN_CLICKED;
        return 0;
    }

    if(btn->state == BTN_CLICKED && event == RELEASE){
        btn->state = BTN_NEUTRAL;
        return 1;
    }
    
    return 0;
}

// Retorna 1 se o mouse está dentro do board ou zero caso contrário
int isInBoard(int mouseX, int mouseY){
    return ((mouseX > BOARDX0 && mouseX < BOARDX1) && (mouseY > BOARDY0 && mouseY < BOARDY1));
}

// Retorna 1 se a troca é permitida
int validSwap(int oriLin, int oriCol, int destLin, int destCol){
    if(abs(oriLin - destLin) == 1 && oriCol == destCol) // Trocas verticais
        return 1;

    if(abs(oriCol - destCol) == 1 && oriLin == destLin) // Trocas horizontais
        return 1;
    
    return 0;
}

// Converte o ponto clicado em indice do board
void getPiecePos(int mouseX, int mouseY, int *px, int *py){
    *px = (mouseX - BOARDX0) / 78;
    *py = (mouseY - BOARDY0) / 79;
}

// Desenha um retângulo em volta da peça selecionada
void drawSelected(int px, int py){
    int dx = 380, dy = 40, tam = 80;
    int x0 = dx + (tam * px), x1 = x0 + tam;
    int y0 = dy + (tam * py), y1 = y0 + tam;
    al_draw_rectangle(x0, y0, x1, y1, al_map_rgb(255, 255, 255), 2);
}

// Desenha a tela com base no valor de gS
void drawScreen(TYPE_STATE game, TYPE_BTN **btn, int **board, coord origin, coord dest, int swap, float dropA, ALLEGRO_BITMAP **icons){
    if(game.state == STATE_MENU){
        drawMenu(btn);
    } else if(game.state == STATE_LEADER){
        drawLeader();
    } else if(game.state == STATE_GAME){
        if(game.inState == SWAP || game.inState == DESWAP){
            drawGame(board, origin, dest, game.level, game.score, dropA, icons, 1);
            if(game.inState == SWAP)
                doSwap(board, origin, dest, swap, icons);
            else
                doSwap(board, dest, origin, swap, icons);
        } else {
            drawGame(board, origin, dest, game.level, game.score, dropA, icons, 0);
        }
    } else if (game.state == STATE_HELP) {
        drawHelp();
    }
}