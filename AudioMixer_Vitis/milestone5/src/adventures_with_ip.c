/*
 * adventures_with_ip.c
 *
 * Main source file. Contains main() and menu() functions.
 */
#include "adventures_with_ip.h"

XIicPs Iic;
XGpio Gpio; // Gpio instance for buttons and switches

XAdcPs XAdcInst;

/* ---------------------------------------------------------------------------- *
 * 									main()										*
 * ---------------------------------------------------------------------------- *
 * Runs all initial setup functions to initialise the audio codec and IP
 * peripherals, before calling the interactive menu system.
 * ---------------------------------------------------------------------------- */
int main(void)
{
	xil_printf("Entering Main\r\n");

	// adc stuff
	xadc_init();
	xil_printf("XADC peripheral configured\r\n");

	//Configure the IIC data structure
	IicConfig(XPAR_XIICPS_0_DEVICE_ID);

	//Configure the Audio Codec's PLL
	AudioPllConfig();
	//Configure the Line in and Line out ports.
	//Call LineInLineOutConfig() for a configuration that enables the HP jack too.
	AudioConfigureJacks();
	xil_printf("\r\nADAU1761 configured");

	/* Initialize GPIO peripherals */
	gpio_init();
	xil_printf("\r\nGPIO peripheral configured");

	/* Initialize interrupt controller */
	irpt_init();
	xil_printf("\r\nInterrupt controller configured");

    xil_printf("\r\nInitialization done");
    xil_printf("\r\n");

    xil_printf("\r\nSelect Track(s) for Control: ");
    xil_printf("\r\n");
    xil_printf("\r\nAvailable Tracks:");
    xil_printf("\r\n    SW7: Track 1");
    xil_printf("\r\n    SW6: Track 2");
    xil_printf("\r\n    SW5: Track 3");
    xil_printf("\r\n    SW4: Track 4");
    xil_printf("\r\n");
    xil_printf("\r\nControls:");
    xil_printf("\r\n    BTNL: Record audio");
    xil_printf("\r\n    BTNC: Playback audio");
    xil_printf("\r\n    BTNR: Reverse audio");
    xil_printf("\r\n    BTND: Combine audio");
    xil_printf("\r\n");

    /* Display interactive menu interface via terminal */
//	menu();
    audio_stream2();
    return 1;
}

/* ---------------------------------------------------------------------------- *
 * 									menu()										*
 * ---------------------------------------------------------------------------- *
 *
 * ---------------------------------------------------------------------------- */
// void menu(){} // menu()


