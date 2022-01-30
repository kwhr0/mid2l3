#include "snd.h"

#define SG_MAX			6
#define BEND_SHIFT		7

typedef struct SG {
	struct SG *next; // must be first member
	u8 *adr;
	u16 id;
	u8 velo, volex, ref, note, regt, regv;
} SG;

static SG sSG[SG_MAX];
static SG *sActiveCh, *sFreeCh;

static const u16 dptable[] = {
	4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4081,
	3852, 3636, 3432, 3239, 3057, 2886, 2724, 2571, 2427, 2290, 2162, 2040,
	1926, 1818, 1716, 1619, 1528, 1443, 1362, 1285, 1213, 1145, 1081, 1020,
	963, 909, 858, 809, 764, 721, 681, 642, 606, 572, 540, 510,
	481, 454, 429, 404, 382, 360, 340, 321, 303, 286, 270, 255,
	240, 227, 214, 202, 191, 180, 170, 160, 151, 143, 135, 127,
	120, 113, 107, 101, 95, 90, 85, 80, 75, 71, 67, 63,
	60, 56, 53, 50, 47, 45, 42, 40, 37, 35, 33, 31,
	30, 28, 26, 25, 23, 22, 21, 20, 18, 17, 16, 15,
	15, 14, 13, 12, 11, 11, 10, 10, 9, 8, 8, 7,
	7, 7, 6, 6, 5, 5, 5, 5,
};

static void SetStep(SG *p, s16 bend) {
	u8 note = (u8)(p->note + (bend >> BEND_SHIFT));
	u8 detune = (u8)(bend & (1 << BEND_SHIFT) - 1);
	u16 dp = (u32)((1 << BEND_SHIFT) - detune) * dptable[note] + 
		(u32)detune * dptable[note + 1] >> BEND_SHIFT;
	p->adr[1] = p->regt;
	p->adr[0] = (u8)dp;
	p->adr[1] = p->regt + 1;
	p->adr[0] = (u8)(dp >> 8);
}

static void SetVol(SG *p) {
	u8 v = (u8)((u16)p->velo * p->volex >> 10);
	if (v > 15) v = 15;
	p->adr[1] = p->regv;
	p->adr[0] = v;
}

void SndKeyOn(u8 prog, u8 note, u8 velo, u8 volex, u8 pan, s16 bend, u16 id) {
	SG *p, *p0, *pn;
	u8 ref, t;
	if ((id & 0xff) == 9) return; // Midi Track 10 is not supported
	ref = (u8)((u16)velo * volex >> 8);
	for (p = sActiveCh; p && p->id != id; p = p->next)
		;
	if (!p) {
		if (sFreeCh) {
			p = sFreeCh;
			sFreeCh = p->next;
		}
		else if (sActiveCh) {
			p = sActiveCh; // smallest ref
			sActiveCh = p->next;
		}
		else return;
		for (p0 = (SG *)&sActiveCh, pn = sActiveCh; pn && ref >= pn->ref; p0 = pn, pn = pn->next)
			;
		p->next = p0->next;
		p0->next = p;
	}
	p->id = id;
	p->velo = velo;
	p->volex = volex;
	p->ref = ref;
	p->note = note;
	SetStep(p, bend);
	SetVol(p);
}

void SndKeyOff(u16 id) {
	SG *p, *p0, *pn;
	for (p0 = (SG *)&sActiveCh, p = sActiveCh; p && p->id != id; p0 = p, p = p->next)
		;
	if (p) {
		p->adr[1] = p->regv;
		p->adr[0] = 0;
		p0->next = p->next;
		p->next = sFreeCh;
		sFreeCh = p;
	}
}

void SndVolex(u8 id_low, u8 volex, u8 pan) {
	SG *p;
	for (p = sActiveCh; p; p = p->next) 
		if ((p->id & 0xff) == id_low) {
			p->volex = volex;
			SetVol(p);
		}
}

void SndBend(u8 id_low, s16 bend) {
	SG *p;
	for (p = sActiveCh; p; p = p->next) 
		if ((p->id & 0xff) == id_low) SetStep(p, bend);
}

void SndInit() {
	u8 *adr = PSG;
	u8 regt = 0, regv = 8;
	SG *p;
	for (p = sSG; p < sSG + SG_MAX; p++) {
		p->next = p + 1;
		p->adr = adr;
		p->regt = regt;
		p->regv = regv++;
		if ((regt += 2) >= 6) {
			adr = PSG + 2;
			regt = 0;
			regv = 8;
		}
	}
	(--p)->next = 0;
	sActiveCh = 0;
	sFreeCh = sSG;
}

void SndExit() {
	PSG[1] = 7; PSG[0] = 0xff;
	PSG[3] = 7; PSG[2] = 0xff;
}
