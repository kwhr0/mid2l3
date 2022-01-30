#include "file.h"
#include "midi.h"
#include "snd.h"
#include <cmoc.h>
#include "xprintf.h"

int main() {
	xdev_out(outscr);
	SndInit();
	FileInit();
	MidiInit();
	MidiHeader();
	u8 t = TICK;
	while (polkbd() == -1 && !MidiUpdate()) {
		if (t != TICK) xputc('+');
		while (t == TICK)
			;
		t = TICK;
	}
	SndExit();
	return 0;
}
