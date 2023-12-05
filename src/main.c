#include <stdio.h>
#include <stdlib.h>
#include "../lib/board.h"
#include "../lib/allib.h"

#define STATE_QUIT -1
#define STATE_MENU 0
#define STATE_LEADER 1
#define STATE_GAME 2
#define STATE_HELP 3
#define STATE_END 4

#define PLAY    0
#define SWAP    1
#define DESWAP  2
#define RESOLVE 3
#define DROP    4
#define VERIFY  5

#define NEW_GAME 0
#define CONTINUE 1
#define LEADER   2
#define QUIT     3

#define RESX 1280
#define RESY 720
#define BOARDX 380
#define BOARDY 40
#define SIZE 8
#define VOLUME 0.5

int main() {
// ================ Inicialização do Allegro ================
    initAllegro();
    
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_DISPLAY* disp = setDisplay(RESX, RESY);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_load_ttf_font("resources/fonts/Technos-PKDZP.ttf", 12, 0);
    must_init(font, "font");

    ALLEGRO_AUDIO_STREAM* songTheme = al_load_audio_stream("resources/audios/stranger-things-124008.wav", 2, 2048);
    must_init(songTheme, "song theme");

    ALLEGRO_AUDIO_STREAM* songGame = al_load_audio_stream("resources/audios/cyberpunk-2099-10701.wav", 2, 2048);
    must_init(songGame, "song game");

    ALLEGRO_SAMPLE* soundSelect = al_load_sample("resources/audios/selectBtn.wav");
    must_init(soundSelect, "soundSelect");

    ALLEGRO_SAMPLE* soundConfirm = al_load_sample("resources/audios/clickBtn.wav");
    must_init(soundConfirm, "soundConfirm");

    ALLEGRO_SAMPLE *soundKonami = al_load_sample("resources/audios/soundKonami.wav");
    must_init(soundKonami, "soundKonami");

    ALLEGRO_SAMPLE *soundPop = al_load_sample("resources/audios/popSound.wav");
     must_init(soundPop, "soundPop");

    ALLEGRO_MOUSE_STATE mouseState; ALLEGRO_EVENT event;

    ALLEGRO_BITMAP **icons = initSprites();
    
    TYPE_BTN **btn = initMenuBtn();

    registerEvents(queue, disp, timer);
    initAudio(songTheme, VOLUME);
    initAudio(songGame, VOLUME);
    al_attach_audio_stream_to_mixer(songTheme, al_get_default_mixer());

// =================== Outras variáveis ==================== 
    bool redraw = true;
    float mouseX, mouseY;    

    TYPE_STATE game;
    game.state = STATE_MENU;
    game.inState = PLAY;
 
    int swapAnimation = 0, mouseBtn = 0, konami = 0;
    int selected, modCombo, end, **board;
    float dropA = 0.0;
    coord origin, dest;

    al_start_timer(timer);
    while (game.state != STATE_QUIT) {
        al_wait_for_event(queue, &event);
        al_get_mouse_state(&mouseState);
        switch (game.state){
// ========================== Menu ==========================
            case STATE_MENU:
                switch (event.type){
                    case ALLEGRO_EVENT_TIMER:
                        if (isOverBtn(btn[NEW_GAME], mouseX, mouseY, soundSelect)) {
                            if(updateBtn(btn[NEW_GAME], mouseClicked(mouseBtn, (mouseState.buttons & 1)))) {
                                al_play_sample(soundConfirm, VOLUME, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                al_drain_audio_stream(songTheme);
                                al_attach_audio_stream_to_mixer(songGame, al_get_default_mixer());
                                board = initBoard(1, &game.score, &game.level, &game.extraPieces);
                                freeMenuBtns(btn);
                                game.state = STATE_GAME;
                            }
                        } else if (isOverBtn(btn[CONTINUE], mouseX, mouseY, soundSelect) && hasSave()) {
                            if(updateBtn(btn[CONTINUE], mouseClicked(mouseBtn, (mouseState.buttons & 1)))) {
                                al_play_sample(soundConfirm, VOLUME, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                al_drain_audio_stream(songTheme);
                                al_attach_audio_stream_to_mixer(songGame, al_get_default_mixer()); 
                                board = initBoard(2, &game.score, &game.level, &game.extraPieces);
                                freeMenuBtns(btn);
                                game.state = STATE_GAME;
                            }
                        } else if (isOverBtn(btn[LEADER], mouseX, mouseY, soundSelect)) {
                            if(updateBtn(btn[LEADER], mouseClicked(mouseBtn, (mouseState.buttons & 1)))) {
                                al_play_sample(soundConfirm, VOLUME, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                game.state = STATE_LEADER;
                            }
                        } else if (isOverBtn(btn[QUIT], mouseX, mouseY, soundSelect)){
                            if(updateBtn(btn[QUIT], mouseClicked(mouseBtn, (mouseState.buttons & 1)))) {
                                al_play_sample(soundConfirm, VOLUME, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                game.state = STATE_QUIT;
                            }
                        }
                        mouseBtn = (mouseState.buttons & 1);
                        redraw = true;
                    break;

                    case ALLEGRO_EVENT_MOUSE_AXES:
                        mouseX = event.mouse.x;
                        mouseY = event.mouse.y;
                    break;

                    case ALLEGRO_EVENT_KEY_DOWN:
                        if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                            game.state = STATE_QUIT;
                        if(event.keyboard.keycode == ALLEGRO_KEY_M){
                            if(al_get_audio_stream_gain(songTheme))
                                al_set_audio_stream_gain(songTheme, 0);
                            else
                                al_set_audio_stream_gain(songTheme, VOLUME);
                        }
                    break;

                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        freeMenuBtns(btn);
                        game.state = STATE_QUIT;
                    break;
                }
            break;

// ======================= Leaderboard ======================
            case STATE_LEADER:
                switch (event.type){
                    case ALLEGRO_EVENT_TIMER:
                        redraw = true;
                    break;

                    case ALLEGRO_EVENT_KEY_CHAR:
                        game.state = STATE_MENU;
                    break;

                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        freeMenuBtns(btn);
                        game.state = STATE_QUIT;
                    break;
                }
            break;

// ========================== Jogo ==========================
            case STATE_GAME: 
                switch (event.type){
                    case ALLEGRO_EVENT_TIMER:
                        // Estado de seleção de peças
                        if (game.inState == PLAY){  
                            if (isInBoard(mouseX, mouseY) && (mouseClicked(mouseBtn, (mouseState.buttons & 1)) == 1)) {
                                int px, py;
                                getPiecePos(mouseX, mouseY, &px, &py); 
                                if(!selected){
                                    getPiece(&origin, py, px);
                                    selected = 1;
                                } else {
                                    getPiece(&dest, py, px);
                                    selected = 0;
                                    if(validSwap(origin.lin, origin.col, dest.lin, dest.col)){
                                        swapAnimation = 0;
                                        game.inState = SWAP;
                                    }
                                }
                            }
                        } 
                        // Estado de troca de peças
                        else if (game.inState == SWAP || game.inState == DESWAP) {
                            swapAnimation += 4;
                            if(swapAnimation == 80){
                                swapAnimation = 0;
                                if(game.inState == SWAP){
                                    swapPieces(board, origin, dest);
                                    game.inState = RESOLVE;
                                } else {
                                    swapPieces(board, dest, origin);
                                    game.inState = PLAY;
                                }
                            }
                        } 
                        // Estado de verificação do movimento
                        else if (game.inState == RESOLVE) {
                            modCombo = 1;
                            if(!isValidMove(board, &origin, &dest, &game.score, game.level, &modCombo)){
                                if(konami == -1) {
                                    game.inState = PLAY; 
                                } else game.inState = DESWAP;
                            } else {
                                al_play_sample(soundPop, VOLUME * 1.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                int hasSpecial = popSpecial(board, &game.score, &game.level, &game.extraPieces, &modCombo);
                                while(hasSpecial)
                                    hasSpecial = popSpecial(board, &game.score, &game.level, &game.extraPieces, &modCombo);
                                game.inState = DROP;
                            }
                        } 
                        // Estado que administra as explosões e reposições do mapa
                        else if (game.inState == DROP){
                                dropA += 4;
                                if(dropA == 60){
                                    dropA = 0.0;
                                    int hasDrops = manageBoard(board, &game.score, &game.level, &game.extraPieces, &modCombo);
                                    if (!hasDrops) game.inState = VERIFY;
                                }
                        }
                        // Estado que verifica se novos trios foram formados
                        else if (game.inState == VERIFY){
                            if(newTrios(board, &game.score, &modCombo, game.level)){
                                al_play_sample(soundPop, VOLUME * 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                int hasSpecial = popSpecial(board, &game.score, &game.level, &game.extraPieces, &modCombo);
                                while(hasSpecial)
                                    hasSpecial = popSpecial(board, &game.score, &game.level, &game.extraPieces, &modCombo);
                                game.inState = DROP;
                            } else {
                                saveGame(board, game.score, game.level, game.extraPieces);
                                if(!hasMoves(board)){
                                    game.state = STATE_END;
                                } else
                                    game.inState = PLAY;
                            }
                        }
                        mouseBtn = (mouseState.buttons & 1);
                        redraw = true;
                    break;

                    case ALLEGRO_EVENT_MOUSE_AXES:
                        mouseX = event.mouse.x;
                        mouseY = event.mouse.y;
                    break;

                    case ALLEGRO_EVENT_KEY_DOWN:
                        if(event.keyboard.keycode == ALLEGRO_KEY_Q)
                            game.state = STATE_END;
                        if(event.keyboard.keycode == ALLEGRO_KEY_H || event.keyboard.keycode == ALLEGRO_KEY_F1)
                            game.state = STATE_HELP;
                        if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                            saveGame(board, game.score, game.level, game.extraPieces);
                            freeBoard(board);
                            game.state = STATE_QUIT;
                        }
                        if(event.keyboard.keycode == ALLEGRO_KEY_M){
                            if(al_get_audio_stream_gain(songGame) != 0)
                                al_set_audio_stream_gain(songGame, 0);
                            else
                                al_set_audio_stream_gain(songGame, VOLUME);
                        }
                        if(konami == 0 || konami == 1){
                            if (event.keyboard.keycode == ALLEGRO_KEY_W) konami++;
                            else konami = 0;
                        } else if(konami == 2 || konami == 3) {
                            if (event.keyboard.keycode == ALLEGRO_KEY_S) konami++;
                            else konami = 0;
                        } else if(konami == 4 || konami == 6) {
                            if (event.keyboard.keycode == ALLEGRO_KEY_A) konami++;
                            else konami = 0;
                        } else if(konami == 5 || konami == 7){
                            if (event.keyboard.keycode == ALLEGRO_KEY_D) konami++;
                            else konami = 0;
                        } else if(konami == 8){
                            if(event.keyboard.keycode == ALLEGRO_KEY_X) konami++;
                            else konami = 0;
                        } else if(konami == 9){
                            if(event.keyboard.keycode == ALLEGRO_KEY_Z) {
                                al_play_sample(soundKonami, VOLUME, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                konami = -1;
                            } else konami = 0;
                        }
                    break;

                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        saveGame(board, game.score, game.level, game.extraPieces);
                        freeBoard(board);
                        game.state = STATE_QUIT;
                    break;
                }
            break;

// ========================== Help ==========================       
            case STATE_HELP:
                switch (event.type){
                    case ALLEGRO_EVENT_KEY_DOWN:
                        game.state = STATE_GAME;
                    break;

                    case ALLEGRO_EVENT_DISPLAY_CLOSE:
                        saveGame(board, game.score, game.level, game.extraPieces);
                        freeBoard(board);
                        game.state = STATE_QUIT;;
                    break;
                }
            break;

// ========================== Fim =========================== 
            case STATE_END:
                switch (event.type){
                    case ALLEGRO_EVENT_TIMER:  
                        game.state = STATE_QUIT;
                        freeBoard(board);
                        end = 1;
                    break;
                }
            break;
        }

// ==================== Desenhar a tela =====================
        if(redraw && al_is_event_queue_empty(queue)){
            drawScreen(game, btn, board, origin, dest, swapAnimation, dropA, icons);
            if(game.state == STATE_GAME && selected)
                drawSelected(origin.col, origin.lin);
            al_flip_display();
            redraw = false;
        }
    }

// ================== Liberação de memória ==================
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(disp);
    al_destroy_font(font);
    deinitAudios(songTheme, songGame, soundSelect, soundConfirm, soundKonami);
    al_destroy_sample(soundPop);
    deinitSprites(icons);
    if(end) {saveScore(game.score);}

    return 0;
}
