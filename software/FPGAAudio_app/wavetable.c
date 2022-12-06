/*
 * wavetable.c
 *
 *  Created on: Nov 10, 2022
 *      Author: ldu08
 */

#include "wavetable.h"

void wavetableConfig() {
	wavetable->VRAM[0] = 0x000000 << 8;
	wavetable->VRAM[1] = 0x004000 << 8;
	wavetable->VRAM[2] = 0x008000 << 8;
	wavetable->VRAM[3] = 0x00c000 << 8;
	wavetable->VRAM[4] = 0x010000 << 8;
	wavetable->VRAM[5] = 0x00c000 << 8;
	wavetable->VRAM[6] = 0x008000 << 8;
	wavetable->VRAM[7] = 0x004000 << 8;

	for (int i=0;i<8;i++) {
		printf("%x\n", wavetable->VRAM[i]);
	}

}

void timingConfig() {
	timing->VRAM[4] = 169/4;//C
	timing->VRAM[3] = 150/4;//D
	timing->VRAM[2] = 134/4;//E
	timing->VRAM[1] = 126/4;//F
	timing->VRAM[0] = 112/4;//G
	timing->VRAM[6] = 100/4;//A
	timing->VRAM[5] = 89/4;//B

	timing->VRAM[7] = 159/4;//C#
	timing->VRAM[8] = 142/4;//D#
	timing->VRAM[9] = 119/4;//F#
	timing->VRAM[10] = 106/4;//G#
	timing->VRAM[11] = 95/4;//A#

	for (int i=0;i<12;i++) {
		printf("%x\n", timing->VRAM[i]);
	}
}


