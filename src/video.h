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

#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h>
#include <cx16.h>
#include <cbm.h>
#include <ascii_charmap.h>
#include <time.h>

#include "constants.h"
#include "game.h"

extern clock_t prevtick;

/**
 * @brief Initialize screen
 * 
 */
void init_screen();

/**
 * @brief Which tile to use for the background
 * 
 * @param tile_id background tile index
 */
void set_background(uint8_t tile_id);

/**
 * @brief Clears the screen
 * 
 * Set background tiles everywhere and sets foreground to transparent
 * 
 */
void clear_screen();

/**
 * @brief Load the tiles from file into memory
 * 
 */
void load_tiles();

/**
 * @brief Build the playing field
 * 
 * Builds the board
 * Places turn and scoring information on screen
 * 
 */
void build_playfield();

/**
 * @brief Set a tile on LAYER1
 * 
 * @param y             y-position of the tile
 * @param x             x-position of the tile
 * @param tile_id       which tile to place
 * @param tile_data     tile placement byte
 */
void set_tile(uint8_t y, uint8_t x, uint8_t tile_id, uint8_t tile_data);

/**
 * @brief Set a background tile (LAYER0)
 * 
 * @param y             y-position of the tile
 * @param x             x-position of the tile
 * @param tile_id       which tile to place
 * @param tile_data     tile placement byte
 */
void set_bgtile(uint8_t y, uint8_t x, uint8_t tile_id, uint8_t tile_data);

/**
 * @brief Write an ASCII-encoded string to the screen
 * 
 * @param str   null-terminated string
 * @param y     y-position to start
 * @param x     x-position to start
 */
void write_string(const char* str, uint8_t y, uint8_t x);

/**
 * @brief Load fontmap into memory
 * 
 * This function automatically unpacks the fontmap encoded as 1bpp into a
 * 8bpp tilemap.
 * 
 * @param col which color for the font to store
 */
void load_fontmap(uint8_t col);

/**
 * @brief Build the playing board
 * 
 * @param size          size of the board
 * @param offset_y      board x-offset with respect to screen
 * @param offset_x      board y-offset with respect to screen
 */
void build_board(uint8_t size, uint8_t offset_y, uint8_t offset_x);

/**
 * @brief Set all foreground tiles (LAYER1) to transparent
 * 
 */
void clear_foreground();

/**
 * @brief Update diagonal background scrolling
 * 
 */
void update_background_diagonal();

/**
 * @brief Associate tile to a sprite
 * 
 * @param sprite_id which sprite to place
 * @param tile_id   which tile to use
 */
void assign_sprite(uint8_t sprite_id, uint8_t tile_id);

/**
 * @brief Position a sprite
 * 
 * @param sprite_id which sprite
 * @param posy      16-pixel y-position
 * @param posx      16-pixel y-position
 */
void set_sprite(uint8_t sprite_id, uint8_t posy, uint8_t posx);

/**
 * @brief Reset all sprites
 * 
 */
void reset_sprites();

/**
 * @brief Set the mouse pointer
 * 
 * @param tile_id   which tile to use
 */
void set_mouse_pointer(uint8_t tile_id);

#endif // _VIDEO_H