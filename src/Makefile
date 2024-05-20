CC=cl65
EMU=~/emulator-linux/x16emu
PYTHON=python3

make: OTHELLO.PRG

OTHELLO.PRG: *.c *.h FONTMAP.DAT TILES.DAT CORRIDORS_OF_TIME.ZSM
	$(CC) -O -o OTHELLO.PRG -t cx16 -C othello.cfg *.c *.s *.lib

TILES.DAT:
	$(PYTHON) ../assets/scripts/convert_tilemap.py TILES.DAT

FONTMAP.DAT:
	$(PYTHON) ../assets/scripts/fontmap.py FONTMAP.DAT

CORRIDORS_OF_TIME.ZSM:
	cp -v ../assets/sound/corridors_of_time.zsm CORRIDORS_OF_TIME.ZSM

run: OTHELLO.PRG
	$(EMU) -prg OTHELLO.PRG -run

clean:
	rm -v *.o *.DAT *.PRG