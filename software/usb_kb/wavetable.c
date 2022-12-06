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
	int div = 2;
	timing->VRAM[4] = 169/div;//C
	timing->VRAM[3] = 150/div;//D
	timing->VRAM[2] = 134/div;//E
	timing->VRAM[1] = 126/div;//F
	timing->VRAM[0] = 112/div;//G
	timing->VRAM[6] = 100/div;//A
	timing->VRAM[5] = 89/div;//B

	timing->VRAM[7] = 159/div;//C#
	timing->VRAM[8] = 142/div;//D#
	timing->VRAM[9] = 119/div;//F#
	timing->VRAM[10] = 106/div;//G#
	timing->VRAM[11] = 95/div;//A#

	for (int i=0;i<12;i++) {
		printf("%x\n", timing->VRAM[i]);
	}
}


