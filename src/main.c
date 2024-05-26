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

#include <cx16.h>
#include <stdint.h>

#include "video.h"
#include "constants.h"
#include "game.h"
#include "menu.h"
#include "mouse.h"
#include "sound.h"

unsigned char keycode;

void main() {
    // load assets into memory
    init_screen();
    load_tiles();

    // load sound engine
    init_sound();
    start_bgmusic();

    // enable mouse
    init_mouse();
    set_mouse_pointer(TILE_MOUSE_CURSOR);

    while(1) {
        while(gamestate == GAME_MENU) {
            clear_screen();
            game_menu();
        }

        while(gamestate == GAME_RUN) {
            switch(current_player) {
                case PLAYER_ONE:
                    switch(player1_type) {
                        case PLAYER_HUMAN:
                            human_turn();
                        break;
                        case PLAYER_CPU:
                            computer_turn();
                        break;
                    }
                break;
                case PLAYER_TWO:
                    switch(player2_type) {
                        case PLAYER_HUMAN:
                            human_turn();
                        break;
                        case PLAYER_CPU:
                            computer_turn();
                        break;
                    }
                break;
            }

            // update sound buffer
            sound_fill_buffers();

            // diagonal background scrolling
            update_background_diagonal();
        }
    }
}