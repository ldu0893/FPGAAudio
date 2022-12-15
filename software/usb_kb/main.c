//ECE 385 USB Host Shield code
//based on Circuits-at-home USB Host code 1.x
//to be used for ECE 385 course materials
//Revised October 2020 - Zuofu Cheng

#include <stdio.h>
#include "system.h"
#include "altera_avalon_spi.h"
#include "altera_avalon_spi_regs.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "usb_kb/GenericMacros.h"
#include "usb_kb/GenericTypeDefs.h"
#include "usb_kb/HID.h"
#include "usb_kb/MAX3421E.h"
#include "usb_kb/transfer.h"
#include "usb_kb/usb_ch9.h"
#include "usb_kb/USB.h"
#include "usb_kb/sgtl5000.h"

extern HID_DEVICE hid_device;

static BYTE addr = 1; 				//hard-wired USB address
const char* const devclasses[] = { " Uninitialized", " HID Keyboard", " HID Mouse", " Mass storage" };

BYTE GetDriverandReport() {
	BYTE i;
	BYTE rcode;
	BYTE device = 0xFF;
	BYTE tmpbyte;

	DEV_RECORD* tpl_ptr;
	printf("Reached USB_STATE_RUNNING (0x40)\n");
	for (i = 1; i < USB_NUMDEVICES; i++) {
		tpl_ptr = GetDevtable(i);
		if (tpl_ptr->epinfo != NULL) {
			printf("Device: %d", i);
			printf("%s \n", devclasses[tpl_ptr->devclass]);
			device = tpl_ptr->devclass;
		}
	}
	//Query rate and protocol
	rcode = XferGetIdle(addr, 0, hid_device.interface, 0, &tmpbyte);
	if (rcode) {   //error handling
		printf("GetIdle Error. Error code: ");
		printf("%x \n", rcode);
	} else {
		printf("Update rate: ");
		printf("%x \n", tmpbyte);
	}
	printf("Protocol: ");
	rcode = XferGetProto(addr, 0, hid_device.interface, &tmpbyte);
	if (rcode) {   //error handling
		printf("GetProto Error. Error code ");
		printf("%x \n", rcode);
	} else {
		printf("%d \n", tmpbyte);
	}
	return device;
}

void setLED(int LED) {
	IOWR_ALTERA_AVALON_PIO_DATA(LEDS_PIO_BASE,
			(IORD_ALTERA_AVALON_PIO_DATA(LEDS_PIO_BASE) | (0x001 << LED)));
}

void clearLED(int LED) {
	IOWR_ALTERA_AVALON_PIO_DATA(LEDS_PIO_BASE,
			(IORD_ALTERA_AVALON_PIO_DATA(LEDS_PIO_BASE) & ~(0x001 << LED)));

}

void printSignedHex0(signed char value) {
	BYTE tens = 0;
	BYTE ones = 0;
	WORD pio_val = IORD_ALTERA_AVALON_PIO_DATA(HEX_DIGITS_PIO_BASE);
	if (value < 0) {
		setLED(11);
		value = -value;
	} else {
		clearLED(11);
	}
	//handled hundreds
	if (value / 100)
		setLED(13);
	else
		clearLED(13);

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
	if (value < 0) {
		setLED(10);
		value = -value;
	} else {
		clearLED(10);
	}
	//handled hundreds
	if (value / 100)
		setLED(12);
	else
		clearLED(12);

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

void setKeycode(BYTE* keycode)
{
	DWORD keycodes;
	keycodes = (keycode[0] << 24) | (keycode[1] << 16) | (keycode[2] << 8) | keycode[3];
	IOWR_ALTERA_AVALON_PIO_DATA(KEYCODE_BASE, keycodes);
}
int main() {
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

		WORD ANA_HP_CTRL = 0x0000;//-? dB
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL, ANA_HP_CTRL);
		printf( "CHIP_ANA_HP_CTRL register: %x\n", SGTL5000_Reg_Rd (i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL));

	wavetableConfig();
	timingConfig();




	BYTE rcode;
	BOOT_MOUSE_REPORT buf;		//USB mouse report
	BOOT_KBD_REPORT kbdbuf;

	BYTE runningdebugflag = 0;//flag to dump out a bunch of information when we first get to USB_STATE_RUNNING
	BYTE errorflag = 0; //flag once we get an error device so we don't keep dumping out state info
	BYTE device;
	WORD keycode;

	printf("initializing MAX3421E...\n");
	MAX3421E_init();
	printf("initializing USB...\n");
	USB_init();
//	BYTE revision = 0x18;
//	BYTE test = MAXreg_rd(revision);
//	printf("Revision: %x\n", test);
	IOWR_ALTERA_AVALON_PIO_DATA(SONG_0_BASE,0);//no song
	IOWR_ALTERA_AVALON_PIO_DATA(SONG_1_BASE,0);//no song
	while (1) {
		WORD CURR_CTRL = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
		SGTL5000_Reg_Wr(i2c_dev, SGTL5000_CHIP_ANA_HP_CTRL, (0x0000 + (CURR_CTRL << 8) + (CURR_CTRL)));
		printf(".");
		MAX3421E_Task();
		USB_Task();
		//usleep (500000);
		if (GetUsbTaskState() == USB_STATE_RUNNING) {
			if (!runningdebugflag) {
				runningdebugflag = 1;
				setLED(9);
				device = GetDriverandReport();
			} else if (device == 1) {
				//run keyboard debug polling
				rcode = kbdPoll(&kbdbuf);
				if (rcode == hrNAK) {
					continue; //NAK means no new data
				} else if (rcode) {
					printf("Rcode: ");
					printf("%x \n", rcode);
					continue;
				}
				printf("keycodes: ");
				for (int i = 0; i < 6; i++) {
					printf("%x ", kbdbuf.keycode[i]);
				}
				setKeycode(&kbdbuf.keycode);
				printSignedHex0(kbdbuf.keycode[0]);
				printSignedHex1(kbdbuf.keycode[1]);
				printf("\n");
			}

			else if (device == 2) {
				rcode = mousePoll(&buf);
				if (rcode == hrNAK) {
					//NAK means no new data
					continue;
				} else if (rcode) {
					printf("Rcode: ");
					printf("%x \n", rcode);
					continue;
				}
				printf("X displacement: ");
				printf("%d ", (signed char) buf.Xdispl);
				printSignedHex0((signed char) buf.Xdispl);
				printf("Y displacement: ");
				printf("%d ", (signed char) buf.Ydispl);
				printSignedHex1((signed char) buf.Ydispl);
				printf("Buttons: ");
				printf("%x\n", buf.button);
				if (buf.button & 0x04)
					setLED(2);
				else
					clearLED(2);
				if (buf.button & 0x02)
					setLED(1);
				else
					clearLED(1);
				if (buf.button & 0x01)
					setLED(0);
				else
					clearLED(0);
			}
		} else if (GetUsbTaskState() == USB_STATE_ERROR) {
			if (!errorflag) {
				errorflag = 1;
				clearLED(9);
				printf("USB Error State\n");
				//print out string descriptor here

			}
		} else //not in USB running state
		{

			printf("USB task state: ");
			printf("%x\n", GetUsbTaskState());
			if (runningdebugflag) {	//previously running, reset USB hardware just to clear out any funky state, HS/FS etc
				runningdebugflag = 0;
				MAX3421E_init();
				USB_init();
			}
			errorflag = 0;
			clearLED(9);
		}

	}
	return 0;
}
