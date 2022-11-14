/*
 * wavetable.c
 *
 *  Created on: Nov 10, 2022
 *      Author: ldu08
 */

#include "wavetable.h"

void wavetableConfig() {
	wavetable->VRAM[0] = 0xf0f0f0f0;
	wavetable->VRAM[1] = 0xff00ff00;
}


