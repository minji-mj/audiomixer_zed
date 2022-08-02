/*
 * userio.h
 *
 */

#ifndef SRC_USERIO_H_
#define SRC_USERIO_H_


//----- defines

// buttons & switches
#define	BUTTON_SWITCH_ID		XPAR_GPIO_1_DEVICE_ID
#define BUTTON_CHANNEL			XGPIO_IR_CH1_MASK	// 1
#define SWITCH_CHANNEL			XGPIO_IR_CH2_MASK	// 2

// interrupt
#define	INTC_DEVICE_ID			XPAR_PS7_SCUGIC_0_DEVICE_ID
#define	INTC_GPIO_INTERRUPT_ID	XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR

// stuff for audio
#define	MAXVAL					500000

//----- prototype functions
unsigned char InterruptConfig();

//----- button masks - 5bits/u8
#define BTNC_MASK   0x01
#define BTND_MASK   0x02
#define BTNL_MASK   0x04
#define BTNR_MASK   0x08
#define BTNU_MASK   0x10

//----- structure for buttons
typedef struct{
	char btnchar;	// for switch.. comment out if not used
	short event;	// button pressed?
	short record;	// boolean
	short playback;	// boolean
} s_Audio;

//----- structure for switches
typedef struct{
	u32 *dataL;
	u32 *dataR;
} s_Track;

//----- structure variables
extern s_Audio audio;
extern s_Track track;


#endif /* SRC_USERIO_H_ */
