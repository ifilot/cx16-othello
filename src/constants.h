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

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// graphics positioning in VRAM
#define TILEBASE            0x0000      // 4*16 tiles at 8bpp
#define FONTBASE            0x4000      // 4*16 tiles at 8bpp
#define MAPBASE0            0xA000      // 64 x 32 tiles
#define MAPBASE1            0xA800      // 64 x 32 tiles
#define PALETTEBASE         0x1FA20
#define PALETTEBYTE         0x00

#define STONE_NONE          0x00
#define STONE_PLAYER1       0x01
#define STONE_PLAYER2       0x02

#define MAPHEIGHT           32
#define MAPWIDTH            32

#define TILE_NONE           0x00
#define TILE_EMPTY          0x00
#define TILE_BG             0x01
#define TILE_EMPTY_CURSOR   0x02
#define TILE_BLACK          0x03
#define TILE_WHITE          0x04
#define TILE_BLACK_CURSOR   0x13
#define TILE_WHITE_CURSOR   0x14

// board offsets
#define BOARD_WOOD          0x20
#define BOARD_STONE         0x30
#define TILE_BEDGLT         0x00
#define TILE_BEDGT          0x01
#define TILE_BEDGRT         0x02
#define TILE_BEDGL          0x03
#define TILE_BC1            0x04
#define TILE_BC2            0x05
#define TILE_BEDGR          0x06
#define TILE_BEDGBL         0x07
#define TILE_BEDGB          0x08
#define TILE_BEDGBR         0x09

// stone colors
#define STONE_RED           0x03
#define STONE_TEAL          0x04
#define STONE_WHITE         0x05
#define STONE_GREY          0x06
#define STONE_YELLOW        0x07
#define STONE_GREEN         0x08
#define STONE_PURPLE        0x09
#define STONE_CYAN          0x0A
#define STONE_ORANGE        0x0B

#define KEYCODE_DOWN        0x91
#define KEYCODE_UP          0x11
#define KEYCODE_LEFT        0x9D
#define KEYCODE_RIGHT       0x1D
#define KEYCODE_RETURN      0x0D
#define KEYCODE_SPACE       0x20
#define KEYCODE_ESCAPE      0x1B

#define PLAYER_ONE          0x00
#define PLAYER_TWO          0x01

#define EDGEFIELD_INACTIVE  0x00
#define EDGEFIELD_ACTIVE    0x01

#define PROBE_NO            0x00
#define PROBE_YES           0x01

#define GAME_STOP           0x00
#define GAME_RUN            0x01
#define GAME_MENU           0x02

#define PLAYER_HUMAN        0x00
#define PLAYER_CPU          0x01

#define COUNT_NO_GAME_END   0x00
#define COUNT_END_GAME      0x01

#define NO                  0x00
#define YES                 0x01

#define RAMBANK (*(uint8_t*)0)

#endif // _CONSTANTS_H