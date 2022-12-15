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
#include "wavetable.h"
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
		WORD CURR_CTRL = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
				SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL, (0x1818 + (CURR_CTRL << 8) + (CURR_CTRL & 0xff)));
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

		WORD ANA_HP_CTRL = 0x0000;//-? dB
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL, ANA_HP_CTRL);
		printf( "CHIP_ANA_HP_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL));



	int i = 0;
	volatile unsigned int *LED_PIO = (unsigned int*)0x170; //make a pointer to access the PIO block
	wavetableConfig();
	timingConfig();
	printf("ALIVE\n");

	while (1) {
		CURR_CTRL = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL, (0x0000 + (CURR_CTRL << 8) + (CURR_CTRL & 0xff)));
		if (CURR_CTRL & 0x200) rhapsody();
		else if (CURR_CTRL & 0x100) cerebrawl();

		DWORD song = 0;
		int space = 100000;

		IOWR_ALTERA_AVALON_PIO_DATA(SONG_0_BASE,song);
		IOWR_ALTERA_AVALON_PIO_DATA(SONG_1_BASE,song);
//		usleep(space*5);
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
	ten = value / 10;
	ones = value % 10;

	pio_val &= 0xFF00;
	pio_val |= (tens << 4);
	pio_val |= (ones << 0);

	IOWR_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE, pio_val);
}

void tempo(QWORD song, int space) {
	IOWR_ALTERA_AVALON_PIO_DATA(SONG_0_BASE, (song >> 32));
	IOWR_ALTERA_AVALON_PIO_DATA(SONG_1_BASE, (song & 0xffffffff));

	 l.l//	printf("%lld\n", song)
	usleep(space);

}

void rhapsody() {
	timing->VRAM[63] = 0;
	QWORD song;
	int trill = 3;
	int space = 60000;
	int s_space = space/5;
	int l_space = space/10;
	song = 0x19; //f3
	tempo(song, space/trill*1.5);
	song = 0x5; //g3
	tempo(song, space/trill*1.4);
	song = 0x19; //f3
	tempo(song, space/trill*1.2);
	song = 0x5; //g3
	tempo(song, space/trill);
	song = 0x19; //f3
	tempo(song, space/trill);
	song = 0x5; //g3
	tempo(song, space/trill);
	song = 0x19; //f3
	tempo(song, space/trill);
	song = 0x5; //g3
	tempo(song, space/trill);
	song = 0x19; //f3
	tempo(song, space/trill);
	song = 0x5; //g3
	tempo(song, space/trill);
	song = 0x19; //f3
	tempo(song, space/trill);
	song = 0x5; //g3
	tempo(song, space/trill);
	song = 0x19; //f3
	tempo(song, space/trill);
	song = 0x5; //g3
	tempo(song, space/trill);
	song = 0x19; //f3
	tempo(song, space/trill);
	song = 0x0;
	tempo(song, l_space);
	rhapsody_gliss();

	song = 0x5b190d1a; //bb5
	tempo(song, space*4);
	song = 0x5a190d1a; //ab5
	tempo(song, space/2);
	song = 0x5b190d1a; //bb5
	tempo(song, space/2);
	song = 0x5a07111f; //ab5
	tempo(song, space*2/3);
	song = 0x5907111f; //gb5
	tempo(song, space*2/3);
	song = 0x5a07111f; //ab5
	tempo(song, space*2/3);
	song = 0x5907111f; //gb5
	tempo(song, space*2/3);
	song = 0x5a07111f; //ab5
	tempo(song, space*2/3);
	song = 0x5907111f; //gb5
	tempo(song, space*2/3);

	song = 0x531b0b14;//F5
	tempo(song, space);
	song = 0x581b0b14;//Eb5
	tempo(song, space);
	song = 0x511b0b10;//D5
	tempo(song, space);
	song = 0x571b0b10;//Db5
	tempo(song, space);
	song = 0x5707050d;//Db5
	tempo(song, space);
	song = 0x5807050d;//Eb5
	tempo(song, space);
	song = 0x52070a0d;//E5
	tempo(song, space);
	song = 0x53070a0d;//F5
	tempo(song, space);

	song = 0x531b190d;//F5
	tempo(song, space);
	song = 0x511b190d;//D5
	tempo(song, space);
	song = 0x2479060d;//Bb4   32nd notes!
	tempo(song, space/4);
	song = 0x5079060d;//C4
	tempo(song, space/4);
	song = 0x2479060d;//Bb4
	tempo(song, space/4);
	song = 0x1c79060d;//A4
	tempo(song, space/4);
	song = 0x2379060d;//Ab4
	tempo(song, space*2);
	song = 0x22731d19;//Gb4
	tempo(song, space);
	song = 0x15731d19;//F4    might need more legato
	tempo(song, space/2);
	song = 0x00731d19;//space
	tempo(song, space/2);
	song = 0x15731d19;//F4
	tempo(song, space);

	song = 0x157b1b0d;//F4
	tempo(song, space-s_space);
	song = 0x15;
	tempo(song, s_space);
	song = 0x15741b050d;//F4
	tempo(song, space*2);
	song = 0x157a070b14;//F4
	tempo(song, space);
	song = 0x157b190d1b;//F4
	tempo(song, space-s_space);
	song = 0x15;
	tempo(song, s_space);
	song = 0x15741b050d;//F4
	tempo(song, space*2);
	song = 0x1571070b14;//F4
	tempo(song, space);

	song = 0x7b190d1b;//D4
	tempo(song, space);
	song = 0x157b190d1b;//F4
	tempo(song, space-s_space);
	song = 0x7b190d1b;
	tempo(song, s_space);
	song = 0x177b190d1b;//G4    trill here
	tempo(song, space/3.2);
	song = 0x237b190d1b;//Ab4
	tempo(song, space/3.2);
	song = 0x177b190d1b;//G4
	tempo(song, space/3.2);
	song = 0x157b190d1b;//F4
	tempo(song, space*1.0625-s_space);
	song = 0x7b190d1b;
	tempo(song, s_space);
	song = 0x177b190d1b;//G4
	tempo(song, space/3.2);
	song = 0x237b190d1b;//Ab4
	tempo(song, space/3.2);
	song = 0x177b190d1b;//G4
	tempo(song, space/3.2);
	song = 0x157b190d1b;//F4
	tempo(song, space*1.0625-s_space);
	song = 0x7b190d1b;
	tempo(song, s_space);
	song = 0x177b190d1b;//G4
	tempo(song, space/3.2);
	song = 0x237b190d1b;//Ab4
	tempo(song, space/3.2);
	song = 0x177b190d1b;//G4
	tempo(song, space/3.2);
	song = 0x157b190d1b;//F4
	tempo(song, space*1.0625);

	song = 0x24;//Bb4
	tempo(song, space);
	song = 0x241d051408;//Bb4
	tempo(song, space*2);
	song = 0x24160b1f15;//Bb4
	tempo(song, space);
	song = 0x24070d2017;//Bb4
	tempo(song, space-s_space);
	song = 0x24;
	tempo(song, s_space);
	song = 0x241d051408;//Bb4
	tempo(song, space*2);
	song = 0x24160b1f15;//Bb4
	tempo(song, space);

	song = 0x070d2017;//G4
	tempo(song, space);
	song = 0x24070d2017;//Bb4
	tempo(song, space-s_space);
	song = 0x070d2017;
	tempo(song, s_space);
	song = 0x50070d2017;//C5
	tempo(song, space/3.2);
	song = 0x57070d2017;//Db5
	tempo(song, space/3.2);
	song = 0x50070d2017;//C5
	tempo(song, space/3.2);
	song = 0x24070d2017;//Bb4
	tempo(song, space*1.0625-s_space);
	song = 0x0070d2017;
	tempo(song, s_space);
	song = 0x50070d2017;//C5
	tempo(song, space/3.2);
	song = 0x57070d2017;//Db5
	tempo(song, space/3.2);
	song = 0x50070d2017;//C5
	tempo(song, space/3.2);
	song = 0x24070d2017;//Bb4
	tempo(song, space*1.0625-s_space);
	song = 0x070d2017;
	tempo(song, s_space);
	song = 0x50070d2017;//C5
	tempo(song, space/3.2);
	song = 0x57070d2017;//Db5
	tempo(song, space/3.2);
	song = 0x50070d2017;//C5
	tempo(song, space/3.2);
	song = 0x24070d2017;//Bb4
	tempo(song, space*1.0625);

	song = 0x58;//Eb5
	tempo(song, space);
	song = 0x581914151c;//Eb5
	tempo(song, space*2);
	song = 0x580a1f2224;//Eb5
	tempo(song, space);
	song = 0x580b202350;//Eb5
	tempo(song, space-s_space);
	song = 0x58;
	tempo(song, s_space);
	song = 0x581914151c;//Eb5
	tempo(song, space*2);
	song = 0x580a1f2224;//Eb5
	tempo(song, space);

	song = 0x0b202350;//C5
	tempo(song, space);
	song = 0x580b202350;//Eb5
	tempo(song, space);
	song = 0x530b202350;//F5
	tempo(song, space/3);
	song = 0x590b202350;//F#5
	tempo(song, space/3);
	song = 0x530b202350;//F5
	tempo(song, space/3);
	song = 0x580b202350;//Eb5
	tempo(song, space);
//	timing->VRAM[63] = 20;
	song = 0x680b202350;//Eb6
	timing->VRAM[63] = 1;
	tempo(song, space*4.5);     //fermata
	song = 0x600b202350;//C6
//	timing->VRAM[63] = 2;
	tempo(song, space*1.5);
	song = 0x530b202350;//F5
	tempo(song, space*1.2);
//	timing->VRAM[63] = 40;
	song = 0x590b202350;//F#5
	tempo(song, space*1.2);
	song = 0x540b202350;//G5
	tempo(song, space*1.3);
	song = 0x5b0b202350;//Bb5
	tempo(song, space*3);

	song = 0x58070d1724;//Eb5
	tempo(song, space*4);

	song = 0;
	tempo(song, space*4);

	timing->VRAM[63] = 0;

}

void test() {
	QWORD song;
	int space = 100000;
	tempo(0x500817001c, space);
//	tempo(0x71000000, space);
//	tempo(0x72000000, space);
//	tempo(0x73000000, space);
//	tempo(0x74000000, space);
//	tempo(0x75000000, space);
//	tempo(0x76000000, space);
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

void rhapsody_gliss() {
	//F3 is 2016
	//A5 is 357
	int space = 300;
	int count = 2016;
	while (count > 378) {
		timing->VRAM[61] = count;
		tempo(0xf0000000, space);
		count--;
	}
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

void cerebrawl() {
	DWORD song;
	int space = 60000;
					// R L
	song = 0x1c110000; //A A
	tempo(song, space);
	song = 0x1c080000;// A E
	tempo(song,space);
	song = 0x50000000; // C 0
	tempo(song, space);
	song  = 0x50080000; // C E
	tempo(song,space);
	song = 0x18060000; // B E
	tempo(song, space);
	song = 0x18100000; // B B
	tempo(song,space);
	song = 0x50000000; //C 0
	tempo(song,space);
	song = 0x50100000; // C B
	tempo(song,space);
	song = 0x58190000; //  D# F
	tempo(song,space);
	song = 0x58110000; // D# A
	tempo(song,space);
	song = 0x58000000; // D# 0
	tempo(song,space);
	song = 0x50110000; // C A
	tempo(song,space);
	song = 0x18060000; // B E
	tempo(song,space);
	song = 0x18100000; // B B
	tempo(song,space);
	song = 0x18000000; // B 0
	tempo(song,space);
	song = 0x18100000; // B B
	tempo (song, space);
	song =  0x55190000;// A F
	tempo(song,space);
	song = 0x55140000; // A C
	tempo(song,space);
	song = 0x55000000; // A 0
	tempo(song,space);
	song = 0x55140000; // A C
	tempo(song,space);
	song = 0x5a060000; // G# E
	tempo(song,space);
	song = 0x55100000; //A B
	tempo(song,space);
	song = 0x56000000; // B 0/
	tempo(song,space);
	song = 0x5a100000; //G# B
	tempo(song,space);
	song = 0x55110000; // A A
	tempo(song,space);
	song = 0x1c060000; // A E
	tempo(song,space);
	song = 0x180000;// B 0
	tempo(song,space);
	song = 0x50060000; // C E
	tempo(song, space);
	song = 0x531b0000; //F D
	tempo(song, space);
	song = 0x53110000; //F A
	tempo(song,space);
	song = 0x53000000; // F 0
	tempo(song,space);
	song = 0x53110000; //F A
	tempo(song,space);
	song = 0x52060000; // E E
	tempo(song, space);
	song = 0x52100000;// E B
	tempo(song,space);
	song = 0x580000; // D# 0
	tempo(song,space);
	song = 0x53100000; // F B
	tempo(song,space);
	song = 0x52060000; // E E
	tempo(song,space);
	song = 0x51100000; //D B
	tempo(song,space);
	song = 0x50000000 ; // C 0
	tempo(song,space);
	song = 0x23060000; // G# E
	tempo(song,space);
	song = 0x1c110000; // A A
	tempo(song,space);
	song = 0x1c060000; // A E
	tempo(song, space);
	song = 0x18000000; //B 0
	tempo(song,space);
	song = 0x50060000; // C E
	tempo(song,space);
	song = 0x531b0000; //F D
		tempo(song, space);
		song = 0x53250000; //F A
		tempo(song,space);
		song = 0x53000000; // F 0
		tempo(song,space);
		song = 0x53110000; //F A
		tempo(song,space);
		song = 0x52060000; // E E
		tempo(song, space);
		song = 0x52100000;// E B
		tempo(song,space);
		song = 0x58000000; // D# 0
		tempo(song,space);
		song = 0x53100000; // F B
		tempo(song,space);
		song = 0x52060000; // E E
		tempo(song,space);
		song = 0x51100000; //D B
		tempo(song,space);
		song = 0x50000000 ; // C 0
		tempo(song,space);
		song = 0x23060000; // G# E
		tempo(song,space);

		song = 0x1c110000; //A A
			tempo(song, space);
			song = 0x1c080000;// A E
			tempo(song,space);
			song = 0x50000000; // C 0
			tempo(song, space);
			song  = 0x50080000; // C E
			tempo(song,space);
			song = 0x18060000; // B E
			tempo(song, space);
			song = 0x18100000; // B B
			tempo(song,space);
			song = 0x50000000; //C 0
			tempo(song,space);
			song = 0x50100000; // C B
			tempo(song,space);
			song = 0x58190000; //  D# F
			tempo(song,space);
			song = 0x58110000; // D# A
			tempo(song,space);
			song = 0x58000000; // D# 0
			tempo(song,space);
			song = 0x50110000; // C A
			tempo(song,space);
			song = 0x18060000; // B E
			tempo(song,space);
			song = 0x18100000; // B B
			tempo(song,space);
			song = 0x18000000; // B 0
			tempo(song,space);
			song = 0x18100000; // B B
			tempo (song, space);
			song =  0x55190000;// A F
			tempo(song,space);
			song = 0x55140000; // A C
			tempo(song,space);
			song = 0x55000000; // A 0
			tempo(song,space);
			song = 0x55140000; // A C
			tempo(song,space);
			song = 0x5a060000; // G# E
			tempo(song,space);
			song = 0x55100000; //A B
			tempo(song,space);
			song = 0x56000000; // B 0/
			tempo(song,space);
			song = 0x5a100000; //G# B
			tempo(song,space);
			song = 0x55110000; // A A
			tempo(song,space);
			song = 0x1c060000; // A E
			tempo(song,space);
			song = 0x180000;// B 0
			tempo(song,space);
			song = 0x50060000; // C E
			tempo(song, space);
			song = 0x531b0000; //F D
			tempo(song, space);
			song = 0x53110000; //F A
			tempo(song,space);
			song = 0x53000000; // F 0
			tempo(song,space);
			song = 0x53110000; //F A
			tempo(song,space);
			song = 0x52060000; // E E
			tempo(song, space);
			song = 0x52100000;// E B
			tempo(song,space);
			song = 0x580000; // D# 0
			tempo(song,space);
			song = 0x53100000; // F B
			tempo(song,space);
			song = 0x52060000; // E E
			tempo(song,space);
			song = 0x51100000; //D B
			tempo(song,space);
			song = 0x500000 ; // C 0
			tempo(song,space);
			song = 0x23060000; // G# E
			tempo(song,space);
			song = 0x1c110000; // A A
			tempo(song,space);
			song = 0x1c060000; // A E
			tempo(song, space);
			song = 0x18000000; //B 0
			tempo(song,space);
			song = 0x50060000; // C E
			tempo(song,space);
			song = 0x531b0000; //F D
				tempo(song, space);
				song = 0x53110000; //F A
				tempo(song,space);
				song = 0x53000000; // F 0
				tempo(song,space);
				song = 0x53110000; //F A
				tempo(song,space);
				song = 0x52060000; // E E
				tempo(song, space);
				song = 0x52100000;// E B
				tempo(song,space);
				song = 0x580000; // D# 0
				tempo(song,space);
				song = 0x53100000; // F B
				tempo(song,space);
				song = 0x52060000; // E E
				tempo(song,space);
				song = 0x51100000; //D B
				tempo(song,space);
				song = 0x500000 ; // C 0
				tempo(song,space);
				song = 0x23060000; // G# E
				tempo(song,space);

		song = 0x1c110800 ; //A A E
		tempo(song,space);
		song = 0x50110800;// C A E
		tempo (song,space);
		song = 0x58110800; //D# A E
		tempo(song,space);
		song = 0x52110800; // E A E
		tempo(song,space);
		song = 0x00110800; // 0 A E
		tempo(song,space);
		song = 0x52110800; //E A E
		tempo(song, space);
		song = 0x00110800; // 0 A E
		tempo(song,space);
		song = 0x52110800; // E A E
		tempo(song,space);
		song = 0x18061000; //B E B
		tempo(song,space);
		song = 0x50061000; //C E B
		tempo(song,space);
		song = 0x1c061000; // A E B
		tempo(song,space);
		song = 0x08061000; // E E B
		tempo(song,space);
		song = 0x00061000; // 0 E B
		tempo(song,space);
		song = 0x08061000; // E E B
		tempo(song,space);
		song = 0x00061000; // 0 E B
		tempo(song, space);
		song = 0x08061000; // E E B
		tempo(song,space);
		song = 0x1c117500 ; //A A A
				tempo(song,space);
				song = 0x50117500;// C A A
				tempo (song,space);
				song = 0x58117500; //D# A A
				tempo(song,space);
				song = 0x52117500; // E A A
				tempo(song,space);
				song = 0x00117500; // 0 A A
				tempo(song,space);
				song = 0x52117500; //E A A
				tempo(song, space);
				song = 0x00117500; // 0 A A
				tempo(song,space);
				song = 0x52117500; // E A A
				tempo(song,space);
				song = 0x18067200; //B E E
				tempo(song,space);
				song = 0x50067200; //C E E
				tempo(song,space);
				song = 0x1c067200; // A E E
				tempo(song,space);
				song = 0x08067200; // E E E
				tempo(song,space);
				song = 0x00067200; // 0 E E
				tempo(song,space);
				song = 0x08067200; // E E E
				tempo(song,space);
				song = 0x00067200; // 0 E E
				tempo(song, space);
				song = 0x08067200; // E E E
		tempo(song,space);
		song = 0x1c750000; // A A
		tempo(song,space/ 4);
		song = 0x50750000; // C A
		tempo(song, space / 4);
		song = 0x52750000; // E A
		tempo(song, space / 4);
		song = 0x55750000; // A A
		tempo(song,space / 4);
		song = 0x52060000; //E E
		tempo(song, space);
		song = 0x52110000;// E A
		tempo(song, space);
		song = 0x52060000;// E E
		tempo(song,space);
		song = 0x18760000; // B B
		tempo(song,space/3);
		song = 0x52760000; // E B
		tempo(song,space / 3);
		song = 0x5a760000; //G# B
		tempo(song,space / 3);
		song = 0x52060000; // E E
		tempo(song, space);
		song = 0x520b0000;// E G#
		tempo(song,space);
		song = 0x52060000; // E E
		tempo(song,space);
		song = 0x54571600 ; //G C# C#
		tempo(song,space);
		song = 0x55060000;// A E
		tempo(song,space);
		song = 0x54110000; //G A
		tempo(song,space);
		song = 0x55060000; // A E
		tempo(song,space);
		song = 0x59511b00; //F# D D
		tempo(song,space);
		song = 0x59511100 ; // F# D A
		tempo(song,space);
		song = 0x59511a00; // F# D D
		tempo(song,space);
		song = 0x59511100; // F# F A
		tempo(song,space);

		song = 0x53511c1b ; // F D A D
		tempo(song,space);
		song = 0x51110000; // D A
		tempo(song,space);
		song = 0x1c1a0000; //A D
		tempo(song,space);
		song = 0x531b0000; // F D
		tempo(song,space);
		song = 0x52750000; // E A
		tempo(song,space);
		song = 0x50060000; // C E
		tempo(song,space);
		song = 0x1c110000; // A A
		tempo(song,space);
		song = 0x52750000; //E A
		tempo(song,space);
		song = 0x58760000; //D# B
		tempo(song,space);
		song = 0x60070000; // C D#
		tempo(song,space);
		song = 0x5610; // B B
		tempo(song,space);
		song = 0x55070a00; // A F# D#
		tempo(song,space);
		song = 0x56060000; // B E
		tempo(song,space);
		song = 0x5a1b0000; // G# D
		tempo(song,space);
		song = 0x531d0000; // F C
		tempo(song,space);
		song = 0x52760000; // E B
		tempo(song,space);


		song = 0x1c750000; // A A
				tempo(song,space/ 4);
				song = 0x50750000; // C A
				tempo(song, space / 4);
				song = 0x52750000; // E A
				tempo(song, space / 4);
				song = 0x55750000; // A A
				tempo(song,space / 4);
				song = 0x52060000; //E E
				tempo(song, space);
				song = 0x52110000;// E A
				tempo(song, space);
				song = 0x52060000;// E E
				tempo(song,space);
				song = 0x18760000; // B B
				tempo(song,space/3);
				song = 0x52760000; // E B
				tempo(song,space / 3);
				song = 0x5a760000; //G# B
				tempo(song,space / 3);
				song = 0x52060000; // E E
				tempo(song, space);
				song = 0x520b0000;// E G#
				tempo(song,space);
				song = 0x52060000; // E E
				tempo(song,space);
				song = 0x54571600 ; //G C# C#
				tempo(song,space);
				song = 0x55060000;// A E
				tempo(song,space);
				song = 0x56110000; //B A
				tempo(song,space);
				song = 0x55060000; // A E
				tempo(song,space);
				song = 0x59511b00; //F# D D
				tempo(song,space);
				song = 0x59511100 ; // F# D A
				tempo(song,space);
				song = 0x59511a00; // F# D D
				tempo(song,space);
				song = 0x59511100; // F# F A
				tempo(song,space);

				song = 0x53511b19; // F D F D
				tempo(song,space);
				song = 0x54110000; // G A
				tempo(song,space);
				song = 0x551a0000; // A D
				tempo(song,space);
				song = 0x601a0000; // C D
				tempo(song,space);
				song = 0x56750000; // B A
				tempo(song,space);
				song = 0x55060000;// A E
				tempo(song,space);
				song = 0x52110000; // E A;
				tempo(song,space);
				song = 0x1c750000; // A A
				tempo(song,space/ 2);
				song = 0x00750000; // 0 A
				tempo(song,space / 2);
				song = 0x1c080000; // A E
				tempo(song,space);
				song = 0x52080000; // E E
				tempo(song,space);
				song = 0x5a521800; // G# E B G#
				tempo(song,space);
				song = 0x52180000; //E G#
				tempo(song,space);
				song = 0x505255111d75; // A E C A E A
				tempo(song, space * 2);

				song = 0x06000000;  // E
				tempo(song, space / 2);
				song = 0x1b000000; // D
				tempo(song, space / 2);
				song = 0x1d000000; // C
				tempo(song, space / 2);
				song = 0x76000000;// B
				tempo(song, space / 2);
				song = 0x75000000; // A
				tempo(song, space);
				song = 0x72000000; // E
				tempo(song, space * 3);
	// C5 - 50 D5 -51 E5 -52 F5 -53 G5 -54 A5 -55 B5 -56
	//C6 - 60
	// C2 - 70
	/*		8'h14;//C
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

