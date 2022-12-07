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

	while (1) {
		mary();

		DWORD song = 0;
		int space = 100000;

		IOWR_ALTERA_AVALON_PIO_DATA(SONG_0_BASE,song);
		usleep(space*5);
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

void tempo(DWORD song, int space) {
	IOWR_ALTERA_AVALON_PIO_DATA(SONG_0_BASE,song);
	usleep(space);

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
		song = 0x17000000; //g
		tempo(song, space/2);
		song = 0;
		tempo(song, space/2);
		song = 0x17000000;//g
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
