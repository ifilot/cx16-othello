;
;                                                                       
;   Author: Ivo Filot <ivo@ivofilot.nl>                                 
;                                                                       
;   CX16-OTHELLO is free software:                                      
;   you can redistribute it and/or modify it under the terms of the     
;   GNU General Public License as published by the Free Software        
;   Foundation, either version 3 of the License, or (at your option)    
;   any later version.                                                  
;                                                                       
;   CX16-OTHELLO is distributed in the hope that it will be useful,     
;   but WITHOUT ANY WARRANTY; without even the implied warranty         
;   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             
;   See the GNU General Public License for more details.                
;                                                                       
;   You should have received a copy of the GNU General Public License   
;   along with this program.  If not, see http://www.gnu.org/licenses/. 
;                                                                       
;

.include "x16.inc"

.export _load_help_assets

.code
;
; Start the sound engine
;
.proc _load_help_assets: near

    ; set ram bank
    lda #2              ; use bank 2, bank 1 is used for sound
    sta $00             ; set ram bank 2

    ; assign file name
    lda #$08            ; filename length
    ldx #<filename      ; low byte filename pointer
    ldy #>filename      ; high byte filename pointer
    jsr X16::Kernal::SETNAM

    ; set file pointer
    lda #2              ; file index
    ldx #8              ; SD-card
    ldy #2              ; headerless load
    jsr X16::Kernal::SETLFS

    ; load file into memory
    lda #0              ; load file into system memory
    ldx #$00
    ldy #$A0            ; banked memory
    jsr X16::Kernal::LOAD

    ; reset ram bank
    lda #0              ; reset ram bank
    sta $00

    rts
.endproc

filename: .asciiz "help.dat"