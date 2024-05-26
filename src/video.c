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

#include "video.h"

clock_t prevtick;

/**
 * @brief Initialize screen
 * 
 */
void init_screen() {
    // set scaling (resolution: 320 x 240)
    VERA.display.hscale = 64;
    VERA.display.vscale = 64;

    // tile configuration
    VERA.layer0.config   = 0x03;                        // 8 bpp color depth
    VERA.layer0.tilebase = (TILEBASE >> 11) | 0x03;     // 16 x 16 tiles
    VERA.layer0.mapbase  = MAPBASE0 >> 9;               // upper 8 bits
    
    VERA.layer1.config   = 0x03;                        // 8 bpp color depth
    VERA.layer1.tilebase = (TILEBASE >> 11) | 0x03;     // 16 x 16 tiles
    VERA.layer1.mapbase  = MAPBASE1 >> 9;

    // enable both layers
    VERA.display.video |= 0b00110000;   

    // enable sprites
    VERA.display.video |= 0b01000000;

    // set time increment
    prevtick = clock();
}

/**
 * @brief Clears the screen
 * 
 * Set background tiles everywhere and sets foreground to transparent
 * 
 */
void clear_screen() {
    uint8_t i = 0, j=0;
    uint32_t map_base_addr;

    // set background tiles
    map_base_addr = MAPBASE0;
    VERA.address = map_base_addr;
    VERA.address_hi = map_base_addr >> 16;
    VERA.address_hi |= 0b10000;

    for (j=0; j<MAPHEIGHT; j++) {
        for (i=0; i<MAPWIDTH; i++) {
            VERA.data0 = TILE_BG;       // background tile
            VERA.data0 = PALETTEBYTE;   // palette offset data
        }
    }

    // set all foreground tiles to transparent
    clear_foreground();
}

/**
 * @brief Load the tiles from file into memory
 * 
 */
void load_tiles() {
    // location of tile set
    uint32_t addr = 0x0000;

    // counters
    uint8_t i = 0, j=0;

    // load tiles into memory
    cbm_k_setnam("tiles.dat");
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(2, TILEBASE);

    // load fontmap into memory
    load_fontmap(0x51);
}

/**
 * @brief Build the playing field
 * 
 * Builds the board
 * Places turn and scoring information on screen
 * 
 */
void build_playfield() {
    // clear the screen
    clear_screen();

    // build the board
    build_board(boardsize, board_offset_y, board_offset_x);

    // write some text on screen
    write_string("TURN", 14, 0);

    // game score "chart"
    write_string("SCORE", 0, 15);
    set_tile(1, 16, stone_color1, PALETTEBYTE);
    set_tile(2, 16, stone_color2, PALETTEBYTE);
}

/**
 * @brief Set a tile on LAYER1
 * 
 * @param y             y-position of the tile
 * @param x             x-position of the tile
 * @param tile_id       which tile to place
 * @param tile_data     tile placement byte
 */
void set_tile(uint8_t y, uint8_t x, uint8_t tile_id, uint8_t tile_data) {
    uint32_t map_base_addr = MAPBASE1 + (y * MAPWIDTH + x) * 2;
    VERA.address = map_base_addr;
    VERA.address_hi = map_base_addr >> 16;
    VERA.address_hi |= 0b10000;
    VERA.data0 = tile_id;
    VERA.data0 = tile_data;
}

/**
 * @brief Set a background tile (LAYER0)
 * 
 * @param y             y-position of the tile
 * @param x             x-position of the tile
 * @param tile_id       which tile to place
 * @param tile_data     tile placement byte
 */
void set_bgtile(uint8_t y, uint8_t x, uint8_t tile_id, uint8_t tile_data) {
    uint32_t map_base_addr = MAPBASE0 + (y * MAPWIDTH + x) * 2;
    VERA.address = map_base_addr;
    VERA.address_hi = map_base_addr >> 16;
    VERA.address_hi |= 0b10000;
    VERA.data0 = tile_id;
    VERA.data0 = tile_data;
}

/**
 * @brief Write an ASCII-encoded string to the screen
 * 
 * @param str   null-terminated string
 * @param y     y-position to start
 * @param x     x-position to start
 */
void write_string(const char* str, uint8_t y, uint8_t x) {
    const uint8_t *s = str;

    uint32_t map_base_addr = MAPBASE1 + (y * MAPWIDTH + x) * 2;
    VERA.address = map_base_addr;
    VERA.address_hi = map_base_addr >> 16;
    VERA.address_hi |= 0b10000;

    while(*s != '\0') {
        if(*s >= ' ' && *s <= '~') {
            VERA.data0 = (*s) - 0x20 + 0x40;
        } else {
            VERA.data0 = 0x40;
        }
        VERA.data0 = 0x00;
        s++;
    }
}

/**
 * @brief Load fontmap into memory
 * 
 * This function automatically unpacks the fontmap encoded as 1bpp into a
 * 8bpp tilemap.
 * 
 * @param col which color for the font to store
 */
void load_fontmap(uint8_t col) {
    // store current rambank
    uint8_t b = RAMBANK;
    uint16_t i = 0;
    uint8_t j = 0, c = 0;
    uint8_t *s = (uint8_t*)0xA000;
    uint32_t map_base_addr = FONTBASE;

    // temporarily store this data on RAMBANK 4
	RAMBANK = 4;

    // load fontmap file into high ram
    cbm_k_setnam("fontmap.dat");
	cbm_k_setlfs(0,8,2);
	cbm_k_load(0,0xA000);

    // "extract" fontmap to video ram
    VERA.address = map_base_addr;
    VERA.address_hi = map_base_addr >> 16;
    VERA.address_hi |= 0b10000;

    for(i=0; i<(1024 * 3); i++) {
        c = *s;
        for(j=0; j<8; j++) {
            VERA.data0 = (c & (1 << j)) ? col : 0;
        }
        s++;
    }

    // revert to old rambank
    RAMBANK = b;
}

/**
 * @brief Build the playing board
 * 
 * @param size          size of the board
 * @param offset_y      board x-offset with respect to screen
 * @param offset_x      board y-offset with respect to screen
 */
void build_board(uint8_t size, uint8_t offset_y, uint8_t offset_x) {
    uint8_t i = 0, j=0;
    uint32_t map_base_addr;

    // game board
    for (j=offset_y ;  j<offset_y + size; j++) {
        map_base_addr = MAPBASE1 + (j * MAPWIDTH + offset_x )  * 2;
        VERA.address = map_base_addr;
        VERA.address_hi = map_base_addr >> 16;
        VERA.address_hi |= 0b10000;
        for (i=0; i<size; i++) {
            if((j + i) % 2 == 0) {
                VERA.data0 = board_type + TILE_BC1;     // TILE
            } else {
                VERA.data0 = board_type + TILE_BC2;     // TILE
            }
            VERA.data0 = PALETTEBYTE;                   // palette offset data
        }
    }

    // build board border
    for (i=0; i<size; i++) {
        // top
        set_tile(offset_y - 1, offset_x + i, board_type + TILE_BEDGT, PALETTEBYTE);

        // left
        set_tile(offset_y + i, offset_x - 1, board_type + TILE_BEDGL, PALETTEBYTE);

        // bottom
        set_tile(offset_y + size, offset_x + i, board_type + TILE_BEDGB, PALETTEBYTE);

        // right
        set_tile(offset_y + i, offset_x + size, board_type + TILE_BEDGR, PALETTEBYTE);

        // left-top
        set_tile(offset_y - 1, offset_x - 1, board_type + TILE_BEDGLT, PALETTEBYTE);

        // right-top
        set_tile(offset_y - 1, offset_x + size, board_type + TILE_BEDGRT, PALETTEBYTE);

        // left-bottom
        set_tile(offset_y + size, offset_x - 1, board_type + TILE_BEDGBL, PALETTEBYTE);

        // right-bottom
        set_tile(offset_y + size, offset_x + size, board_type + TILE_BEDGBR, PALETTEBYTE);
    }
}

/**
 * @brief Set all foreground tiles (LAYER1) to transparent
 * 
 */
void clear_foreground() {
    uint8_t i = 0, j=0;
    uint32_t map_base_addr;

    // set all foreground positions to transparent
    map_base_addr = MAPBASE1;
    VERA.address = map_base_addr;
    VERA.address_hi = map_base_addr >> 16;
    VERA.address_hi |= 0b10000;

    for (j=0; j<MAPHEIGHT; j++) {
        for (i=0; i<MAPWIDTH; i++) {
            VERA.data0 = TILE_NONE;        // transparent tile
            VERA.data0 = PALETTEBYTE;      // palette offset data
        }
    }
}

/**
 * @brief Update diagonal background scrolling
 * 
 */
void update_background_diagonal() {
    clock_t curtick = clock();

    if(((curtick - prevtick) * 1000 / CLOCKS_PER_SEC) > 30) {
        prevtick = curtick;
        VERA.layer0.hscroll = (VERA.layer0.hscroll - 1) % 16;
        VERA.layer0.vscroll = (VERA.layer0.vscroll - 1) % 16;
    }
}

/**
 * @brief Assign a tile to a sprite
 * 
 * @param sprite_id which sprite to place
 * @param tile_id   which tile to use
 */
void assign_sprite(uint8_t sprite_id, uint8_t tile_id) {
    unsigned long sprite_addr = 0x1FC00 + (sprite_id << 3);
    uint32_t graph_addr = TILEBASE + (tile_id << 8);

    VERA.address = sprite_addr;
    VERA.address_hi = sprite_addr >> 16;
    VERA.address_hi |= 0b10000;

    VERA.data0 = graph_addr >> 5;
    VERA.data0 = (graph_addr >> 13) | (1 << 7);
    VERA.data0 = 16;
    VERA.data0 = 0x00;
    VERA.data0 = 16;
    VERA.data0 = 0x00;
    VERA.data0 = 0b00001100;
    VERA.data0 = 0b01010000;
}

/**
 * @brief Set the sprite object
 * 
 * @param sprite_id which sprite
 * @param posy      16-pixel y-position
 * @param posx      16-pixel y-position
 */
void set_sprite(uint8_t sprite_id, uint8_t posy, uint8_t posx) {
    unsigned long sprite_addr = 0x1FC00 | (sprite_id << 3) | 2;

    VERA.address = sprite_addr;
    VERA.address_hi = sprite_addr >> 16;
    VERA.address_hi |= 0b10000;

    VERA.data0 = posx << 4;
    VERA.data0 = posx << 12;
    VERA.data0 = posy << 4;
    VERA.data0 = posy << 12;
}

/**
 * @brief Reset all sprites
 * 
 */
void reset_sprites() {
    uint8_t i;
    unsigned long sprite_addr = 0x1FC00;

    VERA.address = sprite_addr;
    VERA.address_hi = sprite_addr >> 16;
    VERA.address_hi |= 0b10000;

    for(i=0; i<128; i++) {
        VERA.data0 = 0x00;
        VERA.data0 = 0x00;
        VERA.data0 = 16;
        VERA.data0 = 0x00;
        VERA.data0 = 16;
        VERA.data0 = 0x00;
        VERA.data0 = 0b00001100;
        VERA.data0 = 0b01010000;
    }

    set_mouse_pointer(TILE_MOUSE_CURSOR);
}

/**
 * @brief Set the mouse pointer
 * 
 * @param tile_id   which tile to use
 */
void set_mouse_pointer(uint8_t tile_id) {
    unsigned long sprite_addr = 0x1FC00;
    uint32_t graph_addr = TILEBASE + (tile_id << 8);

    VERA.address = sprite_addr;
    VERA.address_hi = sprite_addr >> 16;
    VERA.address_hi |= 0b10000;

    VERA.data0 = graph_addr >> 5;
    VERA.data0 = (graph_addr >> 13) | (1 << 7);
}