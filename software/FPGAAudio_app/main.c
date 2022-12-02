#include "altera_avalon_pio_regs.h"
#include "system.h"
#include <stdio.h>
#include "FPGAAudio/GenericTypeDefs.h"
#include "wavetable.h"
#include <alt_types.h>
int main()
{	printf("1");
	printf("2");
	printf("3");
	printf("4");
	printf("5");
	int i = 0;
	printf("6");
	printf("7");
	volatile unsigned int *LED_PIO = (unsigned int*)0x170; //make a pointer to access the PIO block
	printf("8");
	wavetableConfig();
	printf("9");
	printf("ALIVE\n");


	*LED_PIO = 0; //clear all LEDs

	while ( (1+1) != 3) //infinite loop
	{
		for (i = 0; i < 100000; i++); //software delay
		*LED_PIO |= 0x1; //set LSB
		for (i = 0; i < 100000; i++); //software delay
		*LED_PIO &= ~0x1; //clear LSB
		printSignedHex0(1);
		printSignedHex1(23);
		//wavetableConfig();
	}




	return 1; //never gets here
}

void printSignedHex0(signed char value) {
	BYTE tens = 0;
	BYTE ones = 0;
	WORD pio_val = IORD_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE);
	value = value % 100;
	tens = value / 10;
	ones = value % 10;

	pio_val &= 0x00FF;
	pio_val |= (tens << 12);
	pio_val |= (ones << 8);

	IOWR_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE, pio_val);
}

void printSignedHex1(signed char value) {
	BYTE tens = 0;
	BYTE ones = 0;
	DWORD pio_val = IORD_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE);

	value = value % 100;
	tens = value / 10;
	ones = value % 10;
	tens = value / 10;
	ones = value % 10;

	pio_val &= 0xFF00;
	pio_val |= (tens << 4);
	pio_val |= (ones << 0);

	IOWR_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE, pio_val);
}

void wavetableConfig() {
	printf(" in wavetable config");
	wavetable->VRAM[0] = 0x000000 << 8;
	printf(" in wavetable config 2");
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
