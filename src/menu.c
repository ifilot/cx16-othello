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

/**
 * @brief Show game menu
 * 
 */
void game_menu() {
    static unsigned char keycode;

    write_string("CX16-OTHELLO", 0, 1);
    write_string("1.1.0", 0, 15);

    write_string("(1) PLAYER 1:", 2, 1);
    write_string("(2) PLAYER 2:", 3, 1);
    write_string("(B) BOARD:", 4, 1);

    write_string("(C) TILE COLOR 1:", 11, 1);
    write_string("(V) TILE COLOR 2:", 12, 1);
    write_string("(S) BOARD", 6, 1);
    write_string("    SIZE:", 7, 1);

    write_string("Hit ENTER to start", 14, 1);

    print_choice();

    while(1) {
        // sample keyboard to make adjustments to the game settings
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
            case KEYCODE_RETURN:
                gamestate = GAME_RUN;
                init_game();
            return;
        }

        // update sound buffer
        sound_fill_buffers();
    }
}

/**
 * @brief Print current game configuration to the screen
 * 
 */
void print_choice() {
    char buf[20];
    static const uint8_t offsetx = 12;

    // print whether player 1 is a human or a cpu
    write_string(player1_type == PLAYER_HUMAN ? "HUMAN" : "CPU  ", 2, 14);
    
    // print whether player 2 is a human or a cpu
    write_string(player2_type == PLAYER_HUMAN ? "HUMAN" : "CPU  ", 3, 14);

    // print board style
    write_string(board_type == BOARD_STONE ? "STONE" : "WOOD  ", 4, 12);

    // print board size
    switch(boardsize) {
        case 6:
            memcpy(buf, " 6 x 6 ", 8);
        break;
        case 8:
            memcpy(buf, " 8 x 8 ", 8);
        break;
        case 10:
            memcpy(buf, "10 x 10", 8);
        break;
    }
    write_string(buf, 8, 1);

    // build sample board
    build_board(4, 6, offsetx);

    // print stone colors
    set_tile(7, offsetx+1, stone_color1, 0x00);
    set_tile(7, offsetx+2, stone_color2, 0x00);
    set_tile(8, offsetx+2, stone_color1, 0x00);
    set_tile(8, offsetx+1, stone_color2, 0x00);

    set_tile(11, 18, stone_color1, 0x00);
    set_tile(12, 18, stone_color2, 0x00);
}