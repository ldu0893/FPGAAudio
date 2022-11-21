/*
 * wavetable.c
 *
 *  Created on: Nov 10, 2022
 *      Author: ldu08
 */

#include "wavetable.h"

void wavetableConfig() {
	wavetable->VRAM[0] = 0x00000000;
	wavetable->VRAM[1] = 0x0a000000;
	wavetable->VRAM[2] = 0x00000000;
	wavetable->VRAM[3] = -0x0a000000;

	printf("%x\n", wavetable->VRAM[0]);
	printf("%x\n", wavetable->VRAM[1]);
	printf("%x\n", wavetable->VRAM[2]);
	printf("%x\n", wavetable->VRAM[3]);

}


