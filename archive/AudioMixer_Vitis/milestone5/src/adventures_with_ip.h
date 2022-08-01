/*
 * adventures_with_ip.h
 *
 * Main header file.
 */

#ifndef ADVENTURES_WITH_IP_H_
#define ADVENTURES_WITH_IP_H_

/* ---------------------------------------------------------------------------- *
 * 								Header Files									*
 * ---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdbool.h>
#include <xil_io.h>
#include <sleep.h>
#include "xiicps.h"
#include <xil_printf.h>
#include <xparameters.h>
#include "xgpio.h"
#include "xscugic.h"
#include "xuartps.h"
#include "stdlib.h"

#include "xadcps.h"

/* ---------------------------------------------------------------------------- *
 * 							Custom IP Header Files								*
 * ---------------------------------------------------------------------------- */
#include "audio.h"

/* ---------------------------------------------------------------------------- *
 * 							Prototype Functions									*
 * ---------------------------------------------------------------------------- */
void menu();
//void audio_stream();
void audio_stream2();
void audio_record();
void audio_playback();
unsigned char gpio_init();
unsigned char irpt_init();
//int irptsys_setup(XScuGic);
char btnVal(u32);

unsigned char xadc_init();

/* ---------------------------------------------------------------------------- *
 * 						Redefinitions from xparameters.h 						*
 * ---------------------------------------------------------------------------- */
#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR
#define BUTTON_SWITCH_BASE XPAR_GPIO_1_BASEADDR
#define BUTTON_SWITCH_ID XPAR_GPIO_1_DEVICE_ID

/* ---------------------------------------------------------------------------- *
 * 							Define GPIO Channels								*
 * ---------------------------------------------------------------------------- */
#define BUTTON_CHANNEL XGPIO_IR_CH1_MASK	//1
#define SWITCH_CHANNEL XGPIO_IR_CH2_MASK	//2

/* ---------------------------------------------------------------------------- *
 * 							Audio Scaling Factor								*
 * ---------------------------------------------------------------------------- */
#define SCALE 6

/* ---------------------------------------------------------------------------- *
 * 							Global Variables									*
 * ---------------------------------------------------------------------------- */
extern XIicPs Iic;
extern XGpio Gpio; // Gpio instance for buttons and switches

extern XAdcPs XAdcInst;


/* ---------------------------------------------------------------------------- *
 * 						   Some Other Stuffs									*
 * ---------------------------------------------------------------------------- */
#define RETURN_ON_FAILURE(x) if ((x) != XST_SUCCESS) return XST_FAILURE;

// Button Masks (5 bits)
#define BTNC_MASK   0x0001
#define BTND_MASK   0x0002
#define BTNL_MASK   0x0004
#define BTNR_MASK   0x0008
#define BTNU_MASK   0x0010

// Audio sampling definitions
#define AUDIO_SAMPLING_RATE	48000   // Hz
#define RECORDING_TIME_SECS	10      // s
#define NUM_AUDIO_SAMPLES	(AUDIO_SAMPLING_RATE*RECORDING_TIME_SECS)   // 480000 samples per recording

// Memory stuff
#define BASE_ADDR	XPAR_PS7_DDR_0_S_AXI_BASEADDR	// 0x00100000
#define OFFSET_LR	0x00200000

// Interrupt stuff
//#define BTNSWS_IRPT			XGPIO_IR_MASK	// may need to change. masking all bits
#define INTC_DEVICE_ID		XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_GPIO_IRPT_ID	XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR

//------reorganize


#endif /* ADVENTURES_WITH_IP_H_ */
