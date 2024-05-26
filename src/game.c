/**************************************************************************
 *                                                                        *
 *   Author: Ivo Filot <ivo@ivofilot.nl>                                  *
 *                                                                        *
 *   CX16-OTHELLO is free software:                                       *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   CX16-OTHELLO is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include "game.h"

uint8_t *board = NULL;
uint8_t *edgefield = NULL;
uint8_t *stonecounter = NULL;
int8_t curx = 0;
int8_t cury = 0;
uint8_t current_player = PLAYER_ONE;
uint8_t gamestate = GAME_MENU;
uint8_t player1_type = PLAYER_HUMAN;
uint8_t player2_type = PLAYER_CPU;
uint8_t board_type = BOARD_STONE;
uint8_t stone_color1 = STONE_RED;
uint8_t stone_color2 = STONE_TEAL;
uint8_t boardsize = 8;
uint8_t board_offset_x = 6;
uint8_t board_offset_y = 4;
uint8_t no_move_counter = 0;
uint16_t cpu_waittime = 200;

/**
 * @brief Initialize the game scene
 * 
 */
void init_game() {
    uint8_t i=0, j=0;

    // allocate playfield
    // as a safety precaution, free any existing pointers
    if(board != NULL) {
        free(board);
    }
    if(edgefield != NULL) {
        free(edgefield);
    }
    if(stonecounter != NULL) {
        free(stonecounter);
    }

    // allocate memory and pre-initialize
    board = (uint8_t*)malloc(boardsize*boardsize);
    memset(board, STONE_NONE, boardsize * boardsize);
    edgefield = (uint8_t*)malloc(boardsize*boardsize);
    memset(edgefield, EDGEFIELD_INACTIVE, boardsize * boardsize);
    stonecounter = (uint8_t*)malloc(boardsize*boardsize);

    // reset no move counter
    no_move_counter = 0;

    // set all sprites to transparent
    for(i=0; i<(boardsize * boardsize); i++) {
        assign_sprite(i+1, 0x00);
    }

    // assign cursor sprite
    assign_sprite(SPRITE_TILE_CURSOR, TILE_EMPTY_CURSOR);

    // build the game board
    build_playfield();

    // start starting stones
    set_stone(boardsize/2 - 1, boardsize/2 - 1, STONE_PLAYER1);
    set_stone(boardsize/2 - 1, boardsize / 2, STONE_PLAYER2);
    set_stone(boardsize / 2, boardsize/2 - 1, STONE_PLAYER2);
    set_stone(boardsize / 2, boardsize / 2, STONE_PLAYER1);

    // set the initial score
    count_stones(COUNT_NO_GAME_END);

    // position cursor
    set_cursor(boardsize/2 - 1, boardsize/2 - 1);

    // set turn
    set_tile(14, 5, stone_color1, PALETTEBYTE);

    // set first player
    current_player = PLAYER_ONE;

    // set CPU wait time
    if(player1_type == PLAYER_HUMAN || player2_type == PLAYER_HUMAN) {
        cpu_waittime = 500;
    } else {
        cpu_waittime = 100;
    }

    // start game
    gamestate = GAME_RUN;
}

/**
 * @brief Set a stone tile at the specified position
 * 
 * @param y board y position
 * @param x board x position
 * @param stone which stone to place (STONE_PLAYER1 or STONE_PLAYER2)
 */
void set_stone(uint8_t y, uint8_t x, uint8_t stone) {
    int8_t sx=0, sy=0;
    uint8_t tile_idx = y * boardsize + x;
    uint8_t sprite_id = tile_idx + 1;

    // update board and edgefield
    board[tile_idx] = stone;
    edgefield[tile_idx] = EDGEFIELD_INACTIVE;

    switch(stone) {
        case STONE_PLAYER1:
            assign_sprite(sprite_id, stone_color1);
            set_sprite(sprite_id, y + board_offset_y, x + board_offset_x);
        break;
        case STONE_PLAYER2:
            assign_sprite(sprite_id, stone_color2);
            set_sprite(sprite_id, y + board_offset_y, x + board_offset_x);
        break;
    }

    // update edgefield
    for(sy = (int8_t)y-1; sy <= (int8_t)y+1; sy++) {
        for(sx = (int8_t)x-1; sx <= (int8_t)x+1; sx++) {
            if(sx < 0 || sy < 0 || sx >= boardsize || sy >= boardsize) {
                continue;
            }

            if(board[sy * boardsize + sx] == STONE_NONE) {
                edgefield[sy * boardsize + sx] = EDGEFIELD_ACTIVE;
            }
        }
    }
}

/**
 * @brief Set the cursor
 * 
 * @param y board y position
 * @param x board x position
 */
void set_cursor(uint8_t y, uint8_t x) {
    uint8_t tile = board[cury * boardsize + curx];
    uint8_t edge = edgefield[cury * boardsize + curx];
    uint32_t map_base_addr = 0x0000;

    set_sprite(SPRITE_TILE_CURSOR, board_offset_y + y, board_offset_x + x);

    // update cursor positions
    curx = x;
    cury = y;
}

/**
 * @brief Move cursor
 * 
 * @param y board y position
 * @param x board x position
 */
void move_cursor(int8_t y, int8_t x) {
    uint8_t tile = board[cury * boardsize + curx];

    x += curx;
    y += cury;

    if(x == -1) {
        x = boardsize-1;
    }

    if(x == boardsize) {
        x = 0;
    }

    if(y == -1) {
        y = boardsize-1;
    }

    if(y == boardsize) {
        y = 0;
    }

    set_cursor(y, x);
}

/**
 * @brief Try to place a stone on the board at the currently selected position
 * 
 */
uint8_t place_stone(uint8_t y, uint8_t x, uint8_t probe, uint8_t player) {
    int8_t sx = 0, sy = 0, k = -1;
    uint8_t sum = 0;
    uint8_t dircheck = 0xFF;
    uint8_t stones_turned = 0;
    uint8_t setstone    = (player == PLAYER_ONE ? STONE_PLAYER1 : STONE_PLAYER2);
    uint8_t searchstone = (player == PLAYER_ONE ? STONE_PLAYER2 : STONE_PLAYER1);

    // check if the position is empty
    if(board[y * boardsize + x] != STONE_NONE) {
        return 0;
    }

    // check if there is at least one stone of the opposite color adjacent
    // to this position
    //
    // 0 1 2
    // 3 X 4
    // 5 6 7
    for(sy = (int8_t)y-1; sy <= (int8_t)y+1; sy++) {
        for(sx = (int8_t)x-1; sx <= (int8_t)x+1; sx++) {

            // ignore central point
            if(sy == y && sx == x) {
                continue;
            }

            k++; // note: k starts at -1, so first iteration puts it at 0

            if(sx < 0 || sy < 0 || sx >= boardsize || sy >= boardsize) {
                continue;
            }

            // flip bit to 0 if there is an adjacent stone
            if(board[sy * boardsize + sx] == searchstone) {
                dircheck &= ~(1 << k);
            }
        }
    }

    // if no valid directions, return from this function
    if(dircheck == 0xFF) {
        return 0;
    }

    // loop over directions once more, if a valid direction, perform a 
    // search and turn operation
    for(k=0; k<8; k++) {
        if((dircheck & (1 << k)) == 0) {
            if(k < 3) { // 0, 1, 2
                stones_turned += search_and_turn(y, x, -1, k-1, probe, player);
            } else if(k > 4) { // 5, 6, 7
                stones_turned += search_and_turn(y, x, 1, k-6, probe, player);
            } else { // 3, 4
                stones_turned += search_and_turn(y, x, 0, (k == 3 ? -1 : 1), probe, player);
            }
        }
    }

    // early exit if this was purely meant to probe the number of stones
    // turned
    if(probe == PROBE_YES) {
        return stones_turned;
    }

    // do not swap turns if there were no stones turned
    if(stones_turned == 0) {
        return stones_turned;
    } else {
        // play a sound
        play_thumb();

        // and place the stone
        set_stone(y, x, setstone);
    }

    // change turn
    swap_turn();

    // reset cursor
    set_cursor(y, x);

    // display current number of stones
    count_stones(COUNT_NO_GAME_END);

    return stones_turned;
}

/**
 * @brief Search for neighboring stones to turn and turn them if allowed
 * 
 * @param dy search y-direction
 * @param dx search x-direction
 * @return uint8_t number of stones turned
 */
uint8_t search_and_turn(uint8_t y, uint8_t x, int8_t dy, int8_t dx, uint8_t probe, uint8_t player) {
    uint8_t buffer[20];
    uint8_t *bufferptr = buffer;
    int8_t sx = (int8_t)x + dx;
    int8_t sy = (int8_t)y + dy;
    uint8_t stop = 1;
    uint8_t stones_turned = 0;

    uint8_t searchstone = (player == PLAYER_ONE ? STONE_PLAYER2 : STONE_PLAYER1);
    uint8_t setstone =    (player == PLAYER_ONE ? STONE_PLAYER1 : STONE_PLAYER2);

    while(sx >= 0 && sy >= 0 && sx < boardsize && sy < boardsize && stop == 1) {
        if(board[sy * boardsize + sx] == searchstone) {
            *(bufferptr++) = sy;
            *(bufferptr++) = sx;
        } else if(board[sy * boardsize + sx] == setstone) {
            stop = 0;
        } else {    // empty tile encountered, do not place any stones
            return 0;
        }

        sx += dx;
        sy += dy;
    }

    // check if a "stop stone" was encountered, if not, the loop has terminated
    // early and a 0 should be returned
    if(stop == 1) {
        return 0;
    }

    // set terminating chars
    *(bufferptr++) = 0xFF;
    *(bufferptr++) = 0xFF;

    // place stones
    bufferptr = buffer; // reset pointer to start
    while(*bufferptr != 0xFF) {
        if(probe == PROBE_NO) {
            set_stone(*(bufferptr++), *(bufferptr++), setstone);
        } else {  // just increment the pointer
            bufferptr += 2;
        }
        stones_turned++;
    }

    return stones_turned;
}

/**
 * @brief Count the number of stones for each player on the board and show
 *        this on the screen.
 */
void count_stones(uint8_t end) {
    uint8_t i=0;
    uint8_t black=0, white=0;
    char buf[4];

    for(i=0; i<boardsize * boardsize; i++) {
        switch(board[i]) {
            case STONE_PLAYER1:
                black++;
            break;
            case STONE_PLAYER2:
                white++;
            break;
        }
    }

    itoa(black, buf, 10);
    write_string("   ", 1, 17);
    write_string(buf, 1, 20 - strlen(buf));
    itoa(white, buf, 10);
    write_string("   ", 2, 17);
    write_string(buf, 2, 20 - strlen(buf));

    // check if the board is filled; if so, stop the game
    if(black + white == (boardsize * boardsize) || 
       end == COUNT_END_GAME ||
       black == 0 || white == 0) {
        gamestate = GAME_STOP;
        end_game_state(black, white);
    }
}

/**
 * @brief Perform a computer action
 * 
 */
void computer_turn() {
    uint8_t besty[100];
    uint8_t bestx[100];
    uint8_t bestvalue = 0;
    uint8_t i=0, j=0, k=0;
    clock_t start, next;

    // keep track of time
    start = clock();

    for(j=0; j<boardsize; j++) {
        for(i=0; i<boardsize; i++) {

            // for the active field, calculate the potential number of stones
            // turned when a stone would be placed here
            if(edgefield[j * boardsize + i] == EDGEFIELD_ACTIVE) {
                stonecounter[j * boardsize + i] = place_stone(j, i, PROBE_YES, current_player);
            } else {
                stonecounter[j * boardsize + i] = 0;
            }
        }
    }

    // if any of the corners are an option, increase its value by 50 points
    if(stonecounter[0] > 0) {
        stonecounter[0] += 10;
    }
    if(stonecounter[boardsize - 1] > 0) {
        stonecounter[boardsize - 1] += 10;
    }
    if(stonecounter[boardsize * boardsize - 1] > 0) {
        stonecounter[boardsize * boardsize - 1] += 10;
    }
    if(stonecounter[boardsize * (boardsize - 1)] > 0) {
        stonecounter[boardsize * (boardsize - 1)] += 10;
    }

    // now loop once more over the playfield and pick the option that yields
    // the best result
    for(j=0; j<boardsize; j++) {
        for(i=0; i<boardsize; i++) {
            if(stonecounter[j * boardsize + i] > bestvalue) {
                bestvalue = stonecounter[j * boardsize + i];
            }
        }
    }

    // make another loop and collect all tiles that have the bestvalue as an
    // option
    k = 0;
    for(j=0; j<boardsize; j++) {
        for(i=0; i<boardsize; i++) {
            if(stonecounter[j * boardsize + i] == bestvalue) {
                besty[k] = j;
                bestx[k] = i;
                k++;
            }
        }
    }

    if(bestvalue != 0x00) {
        // add artificial delay
        do {
            next = clock();
            update_background_diagonal();
            sound_fill_buffers();
        }
        while (((next - start) * 1000 / CLOCKS_PER_SEC) < cpu_waittime);

        // randomly select best option from series of best values
        k = rand() % (k + 1);

        place_stone(besty[k], bestx[k], PROBE_NO, current_player);
        no_move_counter = 0;
    } else {
        // no stone can be placed, so turn over to the other player
        no_move_counter++;
        if(no_move_counter == 2) {
            count_stones(COUNT_END_GAME);
        } else {
            swap_turn();
        }
    }
}

void human_turn() {
    static unsigned char keycode = 0x00;
    unsigned char joystat1 = 0xFF;
    unsigned char joystat2 = 0xFF;
    unsigned char joyconn = 0xFF;
    static uint8_t mouse_buttons = 0x00;
    uint16_t *mouse_x = (uint16_t *)0x2;
    uint16_t *mouse_y = (uint16_t *)0x4;
    int8_t ccurx, ccury;

    // first check whether the human can actually perform a valid move
    // if not, we have to swap the turn again (and display an error message)
    if(check_valid_move() == NO) {
        no_move_counter++;
        if(no_move_counter == 2) {
            count_stones(COUNT_END_GAME);
        } else {
            swap_turn();
        }
        return;
    } else {
        no_move_counter = 0;
    }

    // scan keyboard
    asm("jsr $FFE4");
    asm("sta %v", keycode);

    switch(keycode) {
        case KEYCODE_DOWN:
            move_cursor(-1,0);
        return;
        case KEYCODE_UP:
            move_cursor(1,0);
        return;
        case KEYCODE_LEFT:
            move_cursor(0,-1);
        return;
        case KEYCODE_RIGHT:
            move_cursor(0,1);
        return;
        case KEYCODE_SPACE:
            place_stone(cury, curx, PROBE_NO, current_player);
        return;
        case KEYCODE_ESCAPE:
            gamestate = GAME_MENU;
            free_resources();
            reset_sprites();
        return;
    }

    // scan joystick; always scan the joystick belonging to the current player
    read_joystick(current_player+1, &joystat1, &joystat2, &joyconn);

    if(joyconn == 0x00) {
        if(!(joystat1 & (1 << 2))) {
            wait_joystick_release(current_player+1);
            move_cursor(1,0);  // up
            return;
        }
            
        if(!(joystat1 & (1 << 3))) {
            wait_joystick_release(current_player+1);
            move_cursor(-1,0);   // down
            return;
        }
            
        if(!(joystat1 & (1 << 1))) {
            wait_joystick_release(current_player+1);
            move_cursor(0,-1);  // left
            return;
        }

        if(!(joystat1 & (1 << 0))) {
            wait_joystick_release(current_player+1);
            move_cursor(0,1);   // right
            return;
        }

        if(!(joystat1 & (1 << 7))) {
            wait_joystick_release(current_player+1);
            place_stone(cury, curx, PROBE_NO, current_player);
            return;
        }
    }

    // read mouse
    asm("ldx #2");
    asm("jsr $FF6B");
    asm("sta %v", mouse_buttons);

    // determine tile position based based on cursor position
    ccurx = (*mouse_x >> 4) - board_offset_x;
    ccury = (*mouse_y >> 4) - board_offset_y;
    if(ccurx >= 0 && ccurx < boardsize && ccury >=0 && ccury < boardsize) {
        set_cursor(ccury, ccurx);
    }

    if(mouse_buttons & 1) {
        // wait until mouse button is released
        while(mouse_buttons != 0x00) {
            asm("ldx #2");
            asm("jsr $FF6B");
            asm("sta %v", mouse_buttons);
        }
        // place stone in current mouse location
        place_stone(cury, curx, PROBE_NO, current_player);
    }
}

/**
 * @brief Change turn between players
 * 
 */
void swap_turn() {
    if(current_player == PLAYER_ONE) {
        current_player = PLAYER_TWO;
        set_tile(14, 5, stone_color2, PALETTEBYTE);
    } else {
        current_player = PLAYER_ONE;
        set_tile(14, 5, stone_color1, PALETTEBYTE);
    }
}

/**
 * @brief Wait until user pressed space bar
 * 
 */
void wait_till_space() {
    static unsigned char kk = 0;

    while(1) {
        asm("jsr $FFE4");
        asm("sta %v", kk);

        if(kk == KEYCODE_SPACE) {
            break;
        }
    }
}

/**
 * @brief Game has ended, ask user to restart
 * 
 */
void end_game_state(uint8_t black, uint8_t white) {
    static unsigned char keycode = 0x00;

    // clean up game state
    free_resources();

    // show whether the game ended because there are no more valid moves
    if(black + white < (boardsize * boardsize)) {
        write_string("No more moves", 13, 0);
    }

    // display winner 
    if(black > white) {
        write_string("PLAYER 1 WINS!", 14, 0);
    } else if(white > black) {
        write_string("PLAYER 2 WINS!", 14, 0);
    } else {
        write_string("IT'S A TIE!", 14, 0);
    }

    write_string("hit ENTER to", 0, 0);
    write_string("go to menu", 1, 0);

    while(1) {
        asm("jsr $FFE4");
        asm("sta %v", keycode);

        switch(keycode) {
            case KEYCODE_RETURN:
                gamestate = GAME_MENU;
                reset_sprites();
            return;
        }

        sound_fill_buffers();
        update_background_diagonal();
    }
}

/**
 * @brief Assert whether at least a single valid move can be made
 * 
 * @return uint8_t YES when so, else NO (YES and NO are constants)
 */
uint8_t check_valid_move() {
    uint8_t i=0, j=0;
    
    for(j=0; j<boardsize; j++) {
        for(i=0; i<boardsize; i++) {
            if(edgefield[j * boardsize + i] == EDGEFIELD_ACTIVE) {
                if(place_stone(j, i, PROBE_YES, current_player) > 0) {
                    return YES;
                }
            }
        }
    }

    return NO;
}

/**
 * @brief Free resources of board
 * 
 */
void free_resources() {
    uint8_t i=0;
    free(board);
    free(edgefield);
    free(stonecounter);
    board = NULL;
    edgefield = NULL;
    stonecounter = NULL;
}

/**
 * @brief Read joystick
 * 
 * @param joy_id    which joystick
 * @param joystat1  joystick button state 1
 * @param joystat2  joystick button state 2
 * @param joyconn   whether a joystick is connected (0xFF no, 0x00 yes)
 */
void read_joystick(uint8_t joy_id, uint8_t *joystat1, uint8_t *joystat2, 
                   uint8_t *joyconn) {

    // these variables all need to be static
    static uint8_t _joy_id = 0x00;    
    static uint8_t _joystat1 = 0xFF;
    static uint8_t _joystat2 = 0xFF;
    static uint8_t _joyconn = 0xFF;

    // set joystick port
    _joy_id = joy_id;

    asm("lda %v", _joy_id);
    asm("jsr $FF56");
    asm("STA %v", _joystat1);
    asm("STX %v", _joystat2);
    asm("STY %v", _joyconn);

    *joystat1 = _joystat1;
    *joystat2 = _joystat2;
    *joyconn = _joyconn;
}

/**
 * @brief Wait until all joystick buttons have been released
 * 
 * Prevents the system registering multiple button calls
 * 
 * @param joy_id which joystick
 */
void wait_joystick_release(uint8_t joy_id) {
    unsigned char joystat1 = 0xFF;
    unsigned char joystat2 = 0xFF;
    unsigned char joyconn = 0xFF;

    do {
        read_joystick(joy_id, &joystat1, &joystat2, &joyconn);
        //sound_fill_buffers();
    } while(joystat1 != 0xFF || joystat2 != 0xFF);
}