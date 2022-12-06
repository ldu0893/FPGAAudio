/*
 * wavetable.h
 *
 *  Created on: Nov 10, 2022
 *      Author: ldu08
 */

#ifndef WAVETABLE_H_
#define WAVETABLE_H_

#include <system.h>
#include <alt_types.h>

struct WAVETABLE_STRUCT {
	alt_u32 VRAM[8];
};

struct TIMING_STRUCT {
	alt_u16 VRAM[32];
};

static volatile struct WAVETABLE_STRUCT* wavetable = I2S_0_RAM_BASE;
static volatile struct TIMING_STRUCT* timing = I2S_0_TIMING_BASE;

void wavetableConfig();

void timingConfig();


#endif /* WAVETABLE_H_ */
