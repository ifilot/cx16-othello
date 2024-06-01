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

#ifndef _MENU_H
#define _MENU_H

#include <cx16.h>
#include <stdint.h>

#include "video.h"
#include "game.h"
#include "mouse.h"
#include "sound.h"

extern uint8_t helppage;

/**
 * @brief Show game settings menu
 * 
 */
void game_settings();

/**
 * @brief Show game title screen
 * 
 */
void game_title();

/**
 * @brief Show help screen
 * 
 */
void game_help();

/**
 * @brief Print current game configuration to the screen
 * 
 */
void print_choice();

#endif // _MENU_H