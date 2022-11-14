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
	alt_u32 VRAM[2];
};

static volatile struct WAVETABLE_STRUCT* wavetable = I2S_0_BASE;

void wavetableConfig();


#endif /* WAVETABLE_H_ */
