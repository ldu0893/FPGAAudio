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

void twinkle() {
	//50000 eighth
	//10000 quarter
	//20000 half


	DWORD song;
	int space = 50000;   //R L
	song = 0x141d0000; // C C
	tempo(song,space);
	song = 0x00050000; // 0, G
	tempo(song,space);
	song = 0x14060000; //C E
	tempo(song,space);
	song = 0x14050000; //0 G
	tempo(song,space);
	song = 0x171d0000; //G C
	tempo(song,space);
	song = 0x00050000; // 0 G
	tempo(song,space);
	song = 0x17060000; //G E
	tempo(song,space);
	song = 0x17050000; //0 G
	tempo(song,space);
	song = 0x1c1d0000;//A C
	tempo(song,space);
	song = 0x00110000;//0 A
	tempo(song,space);
	song = 0x1c190000;//A F
	tempo(song,space);
	song = 0x1c110000;//0 A;
	tempo(song,space);
	song = 0x171d0000;//G C
	tempo(song,space);
	song = 0x17050000;//0 G
	tempo(song,space);
	song = 0x17060000; //0 E
	tempo(song,space);
	song = 0x00050000;//0 G;
	tempo(song,space);
	song = 0x15051000;//F G B;
	tempo(song,space);
	song = 0x00050000;//0 G
	tempo(song,space);
	song = 0x15190000;//F F
	tempo(song,space);
	song = 0x15050000; //0 G
	tempo(song,space);
	song = 0x081d0000;//E C
	tempo(song,space);
	song = 0x00050000; //0 G
	tempo(song,space);
	song = 0x08060000 ;//E E
	tempo(song,space);
	song = 0x08050000;//0 G
	tempo(song,space);
	song = 0x1a101900; //D B F
	tempo(song,space);
	song = 0x00050000; //0 G
	tempo(song,space);
	song = 0x1a1b0000 ;//D D
	tempo(song,space);
	song = 0x1a190000; //0 F;
	tempo(song,space);
	song = 0x141d0000;//C C
	tempo(song,space);
	song = 0x14050000; // 0 G
	tempo(song,space);
	song = 0x14060000; // 0 E
	tempo(song,space);
	song = 0x00050000; //0 G
	tempo(song,space);

	song = 0x171d0000; //G C
	tempo(song,space);
	song = 0x00050000; // 0 G
	tempo (song,space);
	song = 0x17060000 ;//G E
	tempo(song,space);
	song = 0x17050000; //0 G
	tempo(song,space);
	song = 0x151a1005; // F D B G
	tempo(song,space);
	song = 0x00050000; // 0 G
	tempo(song,space);
	song = 0x15190000; //F F
	tempo(song,space);
	song = 0x15050000;// 0 G
	tempo(song,space);
	song = 0x081d0000;// E C
	tempo(song,space);
	song = 0x00050000; //0 G
	tempo(song,space);
	song = 0x08060000; //E E
	tempo(song,space);
	song = 0x08050000; //0 G
	tempo(song,space);
	song = 0x1a100500; //D B G
	tempo(song,space);
	song = 0x1a050000; //0 G
	tempo(song,space);
	song = 0x1a190000; //0 F
	tempo(song,space);
	song = 0x00050000; // 0 G
	tempo(song,space);

	song = 0x171d0000; //G C
		tempo(song,space);
		song = 0x00050000; // 0 G
		tempo (song,space);
		song = 0x17060000 ;//G E
		tempo(song,space);
		song = 0x17050000; //0 G
		tempo(song,space);
		song = 0x151a1005; // F D B G
		tempo(song,space);
		song = 0x00050000; // 0 G
		tempo(song,space);
		song = 0x15190000; //F F
		tempo(song,space);
		song = 0x15050000;// 0 G
		tempo(song,space);
		song = 0x081d0000;// E C
		tempo(song,space);
		song = 0x00050000; //0 G
		tempo(song,space);
		song = 0x08060000; //E E
		tempo(song,space);
		song = 0x08050000; //0 G
		tempo(song,space);
		song = 0x1a100500; //D B G
		tempo(song,space);
		song = 0x1a050000; //0 G
		tempo(song,space);
		song = 0x1a190000; //0 F
		tempo(song,space);
		song = 0x00050000; // 0 G
		tempo(song,space);

		song = 0x141d0000; // C C
			tempo(song,space);
			song = 0x00050000; // 0, G
			tempo(song,space);
			song = 0x14060000; //C E
			tempo(song,space);
			song = 0x14050000; //0 G
			tempo(song,space);
			song = 0x171d0000; //G C
			tempo(song,space);
			song = 0x00050000; // 0 G
			tempo(song,space);
			song = 0x17060000; //G E
			tempo(song,space);
			song = 0x17050000; //0 G
			tempo(song,space);
			song = 0x1c1d0000;//A C
			tempo(song,space);
			song = 0x00110000;//0 A
			tempo(song,space);
			song = 0x1c190000;//A F
			tempo(song,space);
			song = 0x1c110000;//0 A;
			tempo(song,space);
			song = 0x171d0000;//G C
			tempo(song,space);
			song = 0x17050000;//0 G
			tempo(song,space);
			song = 0x17060000; //0 E
			tempo(song,space);
			song = 0x00050000;//0 G;
			tempo(song,space);
			song = 0x15051000;//F G B;
			tempo(song,space);
			song = 0x00050000;//0 G
			tempo(song,space);
			song = 0x15190000;//F F
			tempo(song,space);
			song = 0x15050000; //0 G
			tempo(song,space);
			song = 0x081d0000;//E C
			tempo(song,space);
			song = 0x00050000; //0 G
			tempo(song,space);
			song = 0x08060000 ;//E E
			tempo(song,space);
			song = 0x08050000;//0 G
			tempo(song,space);
			song = 0x1a101900; //D B F
			tempo(song,space);
			song = 0x00050000; //0 G
			tempo(song,space);
			song = 0x1a1b0000 ;//D D
			tempo(song,space);
			song = 0x1a190000; //0 F;
			tempo(song,space);
			song = 0x141d0000;//C C
			tempo(song,space);
			song = 0x14050000; // 0 G
			tempo(song,space);
			song = 0x14060000; // 0 E
			tempo(song,space);
			song = 0x00050000; //0 G
			tempo(song,space);
	/*
		 8'h14;//C
		 8'h1a;//D
		 8'h08;//E
		 8'h15;//F
		 8'h17;//G
		8'h1c;//A
		 8'h18;//B

		8'h1f;//C#
		 8'h20;//D#
		 8'h22;//F#
		 8'h23;//G#
		  8'h24;//A#

		 8'h1d; C3 //same config, lower reg
		8'h1b;D3
		 8'h6; E3
		 8'h19; F3
		8'h5; G3
		8'h11; A3
		8'h10;B3

		 8'h16; C#3
		 8'h7; D#3
		 8'ha; F#3
		 8'hb; G#3
		 8'hd; A#3
		 */
}
