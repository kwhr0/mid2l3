#ifndef _TYPES_H_
#define _TYPES_H_

#define TICK		(*(/*volatile*/ u8 *)0xe6a5)
#define PSG			((/*volatile*/ u8 *)0xffe4)

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

s16 polkbd(void);
void outscr(u8);
s16 zichr(u8 filenum);

#endif
