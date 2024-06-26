#########################################################################
#                                                                       #
#   Author: Ivo Filot <ivo@ivofilot.nl>                                 #
#                                                                       #
#   CX16-OTHELLO is free software:                                      #
#   you can redistribute it and/or modify it under the terms of the     #
#   GNU General Public License as published by the Free Software        #
#   Foundation, either version 3 of the License, or (at your option)    #
#   any later version.                                                  #
#                                                                       #
#   CX16-OTHELLO is distributed in the hope that it will be useful,     #
#   but WITHOUT ANY WARRANTY; without even the implied warranty         #
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             #
#   See the GNU General Public License for more details.                #
#                                                                       #
#   You should have received a copy of the GNU General Public License   #
#   along with this program.  If not, see http://www.gnu.org/licenses/. #
#                                                                       #
#########################################################################

import numpy as np
import PIL.Image
import os
import sys

ROOT = os.path.dirname(__file__)

def main():
    """
    Open .png file storing an 16x16 bitmap font, extract the relevant characters
    and store this at a 1bpp fontmap file.
    """
    img = PIL.Image.open(os.path.join(ROOT, '..', 'font', '16x16_sm_ascii.png'))
    
    # only grab the 0x40 characters starting at position 0x20
    data = bytearray()
    for i in range(6):
        for j in range(16):
            for y in  range(16):
                b = np.uint8(0x00)
                for k,x in enumerate(range(0,8)):
                    px = img.getpixel((j*16+x, (i+2)*16+y))
                    if px[3] > 150:
                        b |= (1 << k)
                data.append(b)
                b = np.uint8(0x00)
                for k,x in enumerate(range(8,16)):
                    px = img.getpixel((j*16+x, (i+2)*16+y))
                    if px[3] > 150:
                        b |= (1 << k)                
                data.append(b)

    with open(sys.argv[1], 'wb') as f:
        f.write(data)

if __name__ == '__main__':
    main()