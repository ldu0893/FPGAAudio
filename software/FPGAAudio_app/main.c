#include "altera_avalon_pio_regs.h"
#include "system.h"
#include <stdio.h>
#include "FPGAAudio/GenericTypeDefs.h"

#include "altera_avalon_spi.h"
#include "altera_avalon_spi_regs.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_i2c.h"
#include "altera_avalon_i2c_regs.h"
#include "sys/alt_irq.h"
#include "sgtl5000.h"
int main()
{

	ALT_AVALON_I2C_DEV_t *i2c_dev; //pointer to instance structure
		//get a pointer to the Avalon i2c instance
		i2c_dev = alt_avalon_i2c_open("/dev/i2c_0"); //this has to reflect Platform Designer name
		if (NULL==i2c_dev)						     //check the BSP if unsure
		{
			printf("Error: Cannot find /dev/i2c_0\n");
			return 1;
		}
		printf ("I2C Test Program\n");

		alt_avalon_i2c_master_target_set(i2c_dev,0xA); //CODEC at address 0b0001010
		//print device ID (verify I2C is working)
		printf( "Device ID register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_ID));

		//configure PLL, input frequency is 12.5 MHz, output frequency is 180.6336 MHz if 44.1kHz is desired
		//or 196.608 MHz else
		BYTE int_divisor = 180633600/12500000;
		WORD frac_divisor = (WORD)(((180633600.0f/12500000.0f) - (float)int_divisor) * 2048.0f);
		printf( "Programming PLL with integer divisor: %d, fractional divisor %d\n", int_divisor, frac_divisor);
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_PLL_CTRL, \
					int_divisor << SGTL5000_PLL_INT_DIV_SHIFT|
					frac_divisor << SGTL5000_PLL_FRAC_DIV_SHIFT);
		printf( "CHIP_PLL_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_PLL_CTRL));

		//configure power control, disable internal VDDD, VDDIO=3.3V, VDDA=VDDD=1.8V (ext)
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_POWER, \
				SGTL5000_DAC_STEREO|
				SGTL5000_PLL_POWERUP|
				SGTL5000_VCOAMP_POWERUP|
				SGTL5000_VAG_POWERUP|
				SGTL5000_ADC_STEREO|
				SGTL5000_REFTOP_POWERUP|
				SGTL5000_HP_POWERUP|
				SGTL5000_DAC_POWERUP|
				SGTL5000_CAPLESS_HP_POWERUP|
				SGTL5000_ADC_POWERUP);
		printf( "CHIP_ANA_POWER register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_ANA_POWER));

		//select internal ground bias to .9V (1.8V/2)
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_REF_CTRL, 0x004E);
		printf( "CHIP_REF_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_REF_CTRL));

		//enable core modules
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_DIG_POWER,\
				SGTL5000_ADC_EN|
				SGTL5000_DAC_EN|
				//SGTL5000_DAP_POWERUP| //disable digital audio processor in CODEC
				SGTL5000_I2S_OUT_POWERUP|
				SGTL5000_I2S_IN_POWERUP);
		printf( "CHIP_DIG_POWER register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_DIG_POWER));


		//MCLK is 12.5 MHz, configure clocks to use PLL
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_CLK_CTRL, \
				SGTL5000_SYS_FS_44_1k << SGTL5000_SYS_FS_SHIFT |
				SGTL5000_MCLK_FREQ_PLL << SGTL5000_MCLK_FREQ_SHIFT);
		printf( "CHIP_CLK_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_CLK_CTRL));

		//Set as I2S master
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_I2S_CTRL, SGTL5000_I2S_MASTER);
		printf( "CHIP_I2S_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_I2S_CTRL));

		//ADC input from Line
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_CTRL, \
				SGTL5000_ADC_SEL_LINE_IN << SGTL5000_ADC_SEL_SHIFT);
		printf( "CHIP_ANA_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_ANA_CTRL));

		//ADC -> I2S out, I2S in -> DAC
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_SSS_CTRL, \
				SGTL5000_DAC_SEL_I2S_IN << SGTL5000_DAC_SEL_SHIFT |
				SGTL5000_I2S_OUT_SEL_ADC << SGTL5000_I2S_OUT_SEL_SHIFT);
		printf( "CHIP_SSS_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_SSS_CTRL));

		printf( "CHIP_ANA_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_ANA_CTRL));

		//ADC -> I2S out, I2S in -> DAC
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ADCDAC_CTRL, 0x0000);
		printf( "CHIP_ADCDAC_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_ADCDAC_CTRL));
		printf( "CHIP_PAD_STRENGTH register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_PAD_STRENGTH));

		printf( "----------CUSTOM SETTINGS----------\n");
		WORD DAC_VOL = 0x3C3C;//-? dB
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_DAC_VOL, DAC_VOL);
		printf( "CHIP_DAC_VOL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_DAC_VOL));

		WORD ANA_HP_CTRL = 0x1818;//-? dB
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL, ANA_HP_CTRL);
		printf( "CHIP_ANA_HP_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL));



	int i = 0;
	volatile unsigned int *LED_PIO = (unsigned int*)0x170; //make a pointer to access the PIO block
	wavetableConfig();
	timingConfig();
	printf("ALIVE\n");

	while (1) {
		WORD CURR_CTRL = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL, (0x1818 + (CURR_CTRL << 8) + (CURR_CTRL & 0xff)));
		twinkle();

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

void test() {
	DWORD song;
	int space = 100000;
	tempo(0x70000000, space);
	tempo(0x71000000, space);
	tempo(0x72000000, space);
	tempo(0x73000000, space);
	tempo(0x74000000, space);
	tempo(0x75000000, space);
	tempo(0x76000000, space);
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

void among() {
	int space = 100000;
	DWORD song;
	song = 0x08000000; //E
	tempo(song, space);
	song = 0x17000000; //G
	tempo(song,space);
	song = 0x1c000000; //A
	tempo(song,space);
	song = 0x24000000; //Bb
	tempo(song,space);
	song = 0x1c000000; //A
	tempo(song,space);
	song = 0x17000000; //G
	tempo(song,space);
	song = 0x08000000; //E;
	tempo(song, space * 3);
	song = 0x1a000000; //D
	tempo(song, space / 2);
	song = 0x22000000; //F#;
	tempo(song,space / 2);
	song = 0x08000000; //E
	tempo(song, space * 4);
	song = 0;
	tempo(song, space * 2);

	song = 0x08000000; //E
		tempo(song, space);
		song = 0x17000000; //G
		tempo(song,space);
		song = 0x1c000000; //A
		tempo(song,space);
		song = 0x24000000; //Bb
		tempo(song,space);
		song = 0x1c000000; //A
		tempo(song,space);
		song = 0x17000000; //G
		tempo(song,space);
		song = 0x24000000; //Bb
		tempo(song, space *7 / 2);
		song = 0;
		tempo(song, space / 2);
		song = 0x24000000; //Bb
		tempo(song, space * 2 / 3);
		song = 0x1c000000; //A
		tempo(song, space * 2 / 3);
		song = 0x17000000; //G
		tempo(song, space * 2 / 3);

		song = 0x24000000; //Bb
				tempo(song, space * 2 / 3);
				song = 0x1c000000; //A
				tempo(song, space * 2 / 3);
				song = 0x17000000; //G
				tempo(song, space * 2 / 3);
		song = 0x08000000; //E
		tempo(song, space);
		song = 0;
		tempo(song, space);

		song = 0x08000000; //E
			tempo(song, space);
			song = 0x17000000; //G
			tempo(song,space);
			song = 0x1c000000; //A
			tempo(song,space);
			song = 0x24000000; //Bb
			tempo(song,space);
			song = 0x1c000000; //A
			tempo(song,space);
			song = 0x17000000; //G
			tempo(song,space);
			song = 0x08000000; //E;
			tempo(song, space * 3);
			tempo(song, space * 3 / 2);
			song = 0x1a000000; //D
			tempo(song, space / 2);
			song = 0x22000000; //F#;
			tempo(song,space / 2);
			song = 0x08000000; //E
			tempo(song, space * 4);
			song = 0;
			tempo(song, space * 2);

			song = 0x08000000; //E
				tempo(song, space);
				song = 0x17000000; //G
				tempo(song,space);
				song = 0x1c000000; //A
				tempo(song,space);
				song = 0x24000000; //Bb
				tempo(song,space);
				song = 0x1c000000; //A
				tempo(song,space);
				song = 0x17000000; //G
				tempo(song,space);
				song = 0x24000000; //Bb
				tempo(song, space *7 / 2);
				song = 0;
				tempo(song, space / 2);
				song = 0x24000000; //Bb
				tempo(song, space * 2 / 3);
				song = 0x1c000000; //A
				tempo(song, space * 2 / 3);
				song = 0x17000000; //G
				tempo(song, space * 2 / 3);

				song = 0x24000000; //Bb
						tempo(song, space * 2 / 3);
						song = 0x1c000000; //A
						tempo(song, space * 2 / 3);
						song = 0x17000000; //G
						tempo(song, space * 2 / 3);
				song = 0x08000000; //E
				tempo(song, space * 4);
				song = 0;
				tempo(song, space * 4);
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
