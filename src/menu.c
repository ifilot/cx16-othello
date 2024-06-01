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

#include "menu.h"

uint8_t helppage = 0;

/**
 * @brief Show game title screen
 * 
 */
void game_title() {
    static unsigned char keycode;
    static const uint8_t offsetx = 8;

    write_string("CX16-OTHELLO", 0, 0);
    write_string("music by Crisps", 1, 5);
    write_string("1.2.0", 0, 15);

    // build sample board
    build_board(6, 4, offsetx);

    // set sprites
    assign_sprite(1, stone_color1);
    assign_sprite(2, stone_color2);
    assign_sprite(3, stone_color1);
    assign_sprite(4, stone_color2);

    // print stone colors
    set_sprite(1, 6, offsetx+2);
    set_sprite(2, 6, offsetx+3);
    set_sprite(3, 7, offsetx+3);
    set_sprite(4, 7, offsetx+2);

    write_string("(S) SETTINGS", 11, 0);
    write_string("(H) HELP", 12, 0);
    write_string("(ENTER) START", 14, 0);

    while(1) {
        asm("jsr $FFE4");
        asm("sta %v", keycode);

        switch(keycode) {
            case 83:
                gamestate = GAME_SETTINGS;
            return;
            case 72:
                gamestate = GAME_HELP;
            return;
            case KEYCODE_RETURN:
                gamestate = GAME_RUN;
                init_game();
            return;
        }

        // update sound buffer
        sound_fill_buffers();
        update_background_diagonal();
    }
}

/**
 * @brief Show help screen
 * 
 */
void game_help() {
    static unsigned char keycode;
    uint8_t *ptr = (uint8_t*)(0xA000 + 240 * helppage);
    uint8_t x,y;
    uint32_t map_base_addr;
    char buf[4] = {0x00, 0x00, 0x00, 0x00};

    // set sprites
    assign_sprite(1, TILE_NONE);
    assign_sprite(2, TILE_NONE);
    assign_sprite(3, TILE_NONE);
    assign_sprite(4, TILE_NONE);

    write_string("HELP", 0, 0);
    write_string("(N) NEXT (P) PREV", 13, 0);
    write_string("(ESC) BACK  PAGE:", 14, 0);

    // print the page
    asm("lda #2");
    asm("sta $00");
    for(y=1; y<12; y++) {
        map_base_addr = MAPBASE1 + (y * MAPWIDTH) * 2;
        VERA.address = map_base_addr;
        VERA.address_hi = map_base_addr >> 16;
        VERA.address_hi |= 0b10000;

        for(x=0; x<20; x++) {
            if(*ptr >= ' ' && *ptr <= '~') {
                VERA.data0 = (*ptr) - 0x20 + 0x40;
            } else {
                VERA.data0 = 0x40;
            }
            VERA.data0 = 0x00;
            ptr++;
        }
    }
    asm("lda #0");
    asm("sta $00");

    // print page number
    itoa(helppage+1, buf, 10);
    write_string(buf, 14, 17);

    while(1) {
        asm("jsr $FFE4");
        asm("sta %v", keycode);

        switch(keycode) {
            case KEYCODE_ESCAPE:
                gamestate = GAME_MENU;
            return;
            case 78:
                if(helppage < 3) {
                    helppage++;
                }
            return;
            case 80:
                if(helppage > 0) {
                    helppage--;
                }
            return;
        }

        // update sound buffer
        sound_fill_buffers();
        update_background_diagonal();
    }
}

/**
 * @brief Show game menu
 * 
 */
void game_settings() {
    static unsigned char keycode;
    unsigned short *mouse_x = (unsigned short *)0x2;
    unsigned short *mouse_y = (unsigned short *)0x4;

    write_string("SETTINGS", 0, 0);

    write_string("(1) Player 1:", 1, 0);
    write_string("(2) Player 2:", 2, 0);
    write_string("(B) Board:", 4, 0);

    write_string("(S) Size:", 6, 0);

    write_string("(C) Disc player 1:", 9, 0);
    write_string("(V) Disc player 2:", 10, 0);

    write_string("(G) BG scroll:", 12, 0);
    write_string("(M) Music:", 13, 0);

    write_string("(ESCAPE) BACK", 14, 0);

    print_choice();

    while(1) {
        asm("jsr $FFE4");
        asm("sta %v", keycode);

        switch(keycode) {
            case 49:
                player1_type = (player1_type == PLAYER_HUMAN ? PLAYER_CPU : PLAYER_HUMAN);
                print_choice();
            break;
            case 50:
                player2_type = (player2_type == PLAYER_HUMAN ? PLAYER_CPU : PLAYER_HUMAN);
                print_choice();
            break;
            case 66:
                board_type = (board_type == BOARD_STONE ? BOARD_WOOD : BOARD_STONE);
                print_choice();
            break;
            case 71:
                background_scroll = (background_scroll == YES ? NO : YES);
                print_choice();
            break;
            case 77:
                music = (music == YES ? NO : YES);
                if(music == YES) {
                    start_bgmusic();
                } else {
                    stop_bgmusic();
                }
                print_choice();
            break;
            case 67:
                stone_color1++;

                if(stone_color1 == stone_color2) {
                    stone_color1++;
                }

                if(stone_color1 > STONE_ORANGE) {
                    stone_color1 = STONE_RED;
                }

                if(stone_color1 == stone_color2) {
                    stone_color1++;
                }

                print_choice();
            break;
            case 86:
                stone_color2++;

                if(stone_color2 == stone_color1) {
                    stone_color2++;
                }

                if(stone_color2 > STONE_ORANGE) {
                    stone_color2 = STONE_RED;
                }

                if(stone_color2 == stone_color1) {
                    stone_color2++;
                }

                print_choice();
            break;
            case 83:
                boardsize += 2;
                if(boardsize > 10) {
                    boardsize = 6;
                }

                board_offset_x = 10 - (boardsize / 2);
                board_offset_y = 8 - (boardsize / 2);

                print_choice();
            break;
            case KEYCODE_ESCAPE:
                gamestate = GAME_MENU;
            return;
        }

        // update sound buffer
        sound_fill_buffers();
        update_background_diagonal();
    }
}

/**
 * @brief Print current game configuration to the screen
 * 
 */
void print_choice() {
    char buf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    static const uint8_t offsetx = 12;

    // print whether player 1 is a human or a cpu
    write_string(player1_type == PLAYER_HUMAN ? "HUMAN" : "CPU  ", 1, 14);
    
    // print whether player 2 is a human or a cpu
    write_string(player2_type == PLAYER_HUMAN ? "HUMAN" : "CPU  ", 2, 14);

    // print board style
    write_string(board_type == BOARD_STONE ? "STONE" : "WOOD  ", 5, 1);

    // background scrolling
    write_string(background_scroll == YES ? "YES" : "NO  ", 12, 15);

    // music
    write_string(music == YES ? "ON " : "OFF  ", 13, 11);

    // print board size
    switch(boardsize) {
        case 6:
            memcpy(buf, "6 x 6  ", 8);
        break;
        case 8:
            memcpy(buf, "8 x 8  ", 8);
        break;
        case 10:
            memcpy(buf, "10 x 10", 8);
        break;
    }
    write_string(buf, 7, 1);

    // build sample board
    build_board(4, 4, offsetx);

    // set sprites
    assign_sprite(1, stone_color1);
    assign_sprite(2, stone_color2);
    assign_sprite(3, stone_color1);
    assign_sprite(4, stone_color2);

    // print stone colors
    set_sprite(1, 5, offsetx+1);
    set_sprite(2, 5, offsetx+2);
    set_sprite(3, 6, offsetx+2);
    set_sprite(4, 6, offsetx+1);

    set_tile(9, 18, stone_color1, 0x00);
    set_tile(10, 18, stone_color2, 0x00);
}