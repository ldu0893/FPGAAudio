/*
 * wavetable.c
 *
 *  Created on: Nov 10, 2022
 *      Author: ldu08
 */

#include "wavetable.h"

void wavetableConfig() {
	wavetable->VRAM[0] = 0xff00ff00;
	wavetable->VRAM[1] = 0x00000000;

	printf("%x\n", wavetable->VRAM[0]);
	printf("%x\n", wavetable->VRAM[1]);
}


