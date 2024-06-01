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

#ifndef _GAME_H
#define _GAME_H

#include <cx16.h>
#include <stdlib.h>
#include <string.h>
#include <ascii_charmap.h>
#include <stdio.h>
#include <time.h>

#include "constants.h"
#include "video.h"
#include "sound.h"
#include "sound_low.h"
#include "mouse.h"

extern uint8_t *board;              // store board configuration
extern uint8_t *edgefield;          // which board positions are 'active'
extern uint8_t *stonecounter;       // how many stones to gain per board pos
extern int8_t curx;                 // cursor position x
extern int8_t cury;                 // cursor position y
extern uint8_t current_player;      // current player
extern uint8_t gamestate;           // curent gamestate (GAME_STOP, GAME_RUN, GAME_MENU)
extern uint8_t player1_type;        // whether player 1 is human or cpu
extern uint8_t player2_type;        // whether player 2 is human or cpu
extern uint8_t board_type;          // which board type (stone or wood)
extern uint8_t stone_color1;        // color of stones for player 1
extern uint8_t stone_color2;        // color of stones for player 2
extern uint8_t boardsize;           // board size (6, 8 or 10)
extern uint8_t board_offset_x;      // board x-offset with respect to screen
extern uint8_t board_offset_y;      // board y-offset with respect to screen
extern uint8_t no_move_counter;     // how many turns in a row no move can be made
extern uint16_t cpu_waittime;       // time to wait until the computer makes a move
extern uint8_t background_scroll;   // whether or not to perform background scrolling

/**
 * @brief Initialize the game scene
 * 
 */
void init_game();

/**
 * @brief Set a stone tile at the specified position
 * 
 * @param y board y position
 * @param x board x position
 * @param stone which stone to place 
 */
void set_stone(uint8_t y, uint8_t x, uint8_t stone);

/**
 * @brief Set the cursor
 * 
 * @param y board y position
 * @param x board x position
 */
void set_cursor(uint8_t y, uint8_t x);

/**
 * @brief Move cursor
 * 
 * @param y board y position
 * @param x board x position
 */
void move_cursor(int8_t y, int8_t x);

/**
 * @brief Try to place a stone on the board at (y,x)
 * 
 * @param probe whether to just count the number of turned stones
 * @return uint8_t number of stones turned
 */
uint8_t place_stone(uint8_t y, uint8_t x, uint8_t probe, uint8_t player);

/**
 * @brief Search for neighboring stones to turn and turn them if allowed
 *
 * @param y start y-position
 * @param x start x-direction
 * @param dy search y-direction
 * @param dx search x-direction
 * @return uint8_t number of stones turned
 */
uint8_t search_and_turn(uint8_t y, uint8_t x, int8_t dy, int8_t dx, uint8_t probe, uint8_t player);

/**
 * @brief Count the number of stones for each player on the board and show
 *        this on the screen.
 * 
 * When end is set to 1, the game will end
 */
void count_stones(uint8_t end);

/**
 * @brief Perform a computer action
 * 
 */
void computer_turn();

void human_turn();

/**
 * @brief Change turn between players
 * 
 */
void swap_turn();

/**
 * @brief Wait until user pressed space bar
 * 
 */
void wait_till_space();

/**
 * @brief Assert whether at least a single valid move can be made
 * 
 * @return uint8_t YES when so, else NO (YES and NO are constants)
 */
uint8_t check_valid_move();

/**
 * @brief Game has ended, ask user to restart
 * 
 */
void end_game_state(uint8_t black, uint8_t white);

/**
 * @brief Free resources of board
 * 
 */
void free_resources();

/**
 * @brief Read joystick
 * 
 * @param joy_id    which joystick
 * @param joystat1  joystick button state 1
 * @param joystat2  joystick button state 2
 * @param joyconn   whether a joystick is connected (0xFF no, 0x00 yes)
 */
void read_joystick(uint8_t joy_id, uint8_t *joystat1, uint8_t *joystat2, 
                   uint8_t *joyconn);

/**
 * @brief Wait until all joystick buttons have been released
 * 
 * Prevents the system registering multiple button calls
 * 
 * @param joy_id which joystick
 */
void wait_joystick_release(uint8_t joy_id);

#endif