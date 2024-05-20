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

.scope zsmkit
.include "zsmkit.inc"
.endscope

.export _init_sound
.export _start_bgmusic
.export _stop_bgmusic
.export _rewind_bgmusic
.export _sound_fill_buffers
.export _play_thumb

TONE1 = 1189
TONE2 = 800
CHANNEL = 0

.code
;
; Start the sound engine
;
.proc _init_sound: near
    jsr zsmkit::zsm_init_engine     ; initialize engine
    jsr zsmkit::zsmkit_setisr

	ldx #0			; priority
	lda #<filename  ; filename low byte
	ldy #>filename  ; high byte
	jsr zsmkit::zsm_setfile

	ldx #0			; priority
	lda #$20		; attenuation value
	jsr zsmkit::zsm_setatten

    rts
.endproc

;
; Start background music
;
.proc _start_bgmusic: near
	ldx #0
	jsr zsmkit::zsm_play
	rts
.endproc

;
; Stop background music
;
.proc _stop_bgmusic: near
	ldx #0
	jsr zsmkit::zsm_stop
	rts
.endproc

;
; Rewind background music
;
.proc _rewind_bgmusic: near
	ldx #0
	jsr zsmkit::zsm_rewind
	rts
.endproc

;
; Fill sound buffers
;
.proc _sound_fill_buffers: near
	jmp zsmkit::zsm_fill_buffers
.endproc

;
; Play short sound when placing down a stone
;
; Operates on fixed channel set by CHANNEL variable
;
.proc _play_thumb: near
   ldx #<TONE1
   ldy #>TONE1
   jsr play_note
   wai
   wai
   wai
   wai
   wai
   jsr sound_off
   rts

;
; Play single note
;
; X - low byte of the note
; Y - high byte of the note
;
; Operates on fixed channel set by CHANNEL variable
;
play_note:
   lda #<(Vera::VRAM_psg + CHANNEL * 4)
   sta Vera::Reg::AddrL
   lda #>(Vera::VRAM_psg + CHANNEL * 4)
   sta Vera::Reg::AddrM
   lda #%00010001
   sta Vera::Reg::AddrH

   stx Vera::Reg::Data0
   sty Vera::Reg::Data0
   lda #$FF
   sta Vera::Reg::Data0
   lda #((2 << 6) | %011111)
   sta Vera::Reg::Data0
   rts

;
; Turn off sound engine
;
; Operates on fixed channel set by CHANNEL variable
;
sound_off:
   lda #<(Vera::VRAM_psg + CHANNEL * 4 + 2)
   sta Vera::Reg::AddrL
   lda #>(Vera::VRAM_psg + CHANNEL * 4)
   sta Vera::Reg::AddrM
   lda #%00010001
   sta Vera::Reg::AddrH
   lda 0
   sta Vera::Reg::Data0
   rts
.endproc

filename: .asciiz "cot.zsm"