#include "altera_avalon_pio_regs.h"
#include "system.h"
#include <stdio.h>
#include "FPGAAudio/GenericTypeDefs.h"
int main()
{
	int i = 0;
	volatile unsigned int *LED_PIO = (unsigned int*)0x170; //make a pointer to access the PIO block
	wavetableConfig();
	timingConfig();
	printf("ALIVE\n");


//	*LED_PIO = 0; //clear all LEDs
	while ( (1+1) != 3) { //infinite loop
//	{
//		for (i = 0; i < 100000; i++); //software delay
//		*LED_PIO |= 0x1; //set LSB
//		for (i = 0; i < 100000; i++); //software delay
//		*LED_PIO &= ~0x1; //clear LSB
////		printSignedHex0(1);
////		printSignedHex1(23);
//	}
	mary();
	}
	//song == 0x04070a00;
	/*

	 'h14;//C
	'h1a;//D
	'h08;//E
	'h15;//F
	'h17;//G
	'h1c;//A
	'h18;//B
	'h1f;//C#
	'h20;//D#
	'h22;//F#
	'h23;//G#
	'h24;//A#
	 */


//	IOWR_ALTERA_AVALON_PIO_DATA(SONG_0_BASE, song);


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

void tempo(DWORD song, int space) {
	usleep(space);
	IOWR_ALTERA_AVALON_PIO_DATA(SONG_0_BASE,song);

}
void mary() {
	DWORD song;
	int space = 100000;
		song = 0x08000000; //e
		tempo(song, space);
		song = 0x1a000000; // d
		tempo(song, space);
		song = 0x14000000; //c
		tempo(song, space);
		song = 0x1a000000; //d
		tempo(song, space);
		song = 0x08000000; //e
		tempo(song, space/2);
		song = 0;
		tempo(song, space/2);
		song = 0x08000000; //e
		tempo(song, space/2);
		song = 0;
		tempo(song, space/2);
		song = 0x08000000; //e
		tempo(song, space);
		song = 0;
		tempo(song, space);
		song = 0x1a000000; //d
		tempo(song, space/2);
		song = 0;
		tempo(song, space/2);
		song = 0x1a000000; //d
		tempo(song, space/2);
		song = 0;
		tempo(song, space/2);
		song = 0x1a000000; //d
		tempo(song, space);
		song = 0;
		tempo(song, space);
		song = 0x08000000;// e
		tempo(song, space);
		song = 0x0a000000; //g
		tempo(song, space/2);
		song = 0;
		tempo(song, space/2);
		song = 0x0a000000;//g
		tempo(song, space);
		song = 0;
		tempo(song, space);
		song = 0x08000000; //e
		tempo(song, space);
		song = 0x1a000000; //d
		tempo(song, space);
		song = 0x14000000; //c
		tempo(song, space);
		song = 0x1a000000; //d
		tempo(song, space);
		song = 0x08000000; //e
		tempo(song,space/2);
		song = 0;
		tempo(song, space / 2);
		song = 0x08000000; //e
		tempo(song, space/2);
		song = 0;
		tempo(song,space/2);
		song = 0x08000000; // e
		tempo(song,space);
		song = 0;
		tempo(song,space);
		song = 0x1a000000; //d
		tempo(song,space / 2);
		song = 0;
		tempo(song,space/2);
		song = 0x1a000000; //d
		tempo(song,space);
		song = 0x08000000; //e
		tempo(song,space);
		song = 0x1a000000; //d
		tempo(song,space);
		song = 0x14000000; //c
		tempo(song,space);
		song = 0;
		tempo(song,space);
}
