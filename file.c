#include "file.h"

static s32 sRemain;

void FileInit() {
	sRemain = -1;
}

s16 FileGetChar() {
	s16 c;
	if (sRemain && (c = zichr(1)) != -1) {
		sRemain--;
		return c;
	}
	return -1;
}

void FileSetRemain(u32 r) {
	sRemain = r;
}
