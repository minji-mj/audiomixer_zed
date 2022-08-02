/*
 * userio.c
 *
 */
#include "audiomixer.h"
#include "userio.h"

XScuGic INTCInst;

//----- prototype functions
// user inputs
void BTN_Event();

// interrupts
int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr);
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
void BTN_Intr_Handler(void *InstancePtr);

//----- find button interrupt
void BTN_Event(){
	static u32 btn_prev = 0;
	u8 btn, btnRd;

	audio.record = 0;
	audio.playback = 0;

	btnRd = XGpio_DiscreteRead(&Gpio, BUTTON_CHANNEL);
	btn = btnRd & (BTNC_MASK|BTND_MASK|BTNL_MASK|BTNR_MASK|BTNU_MASK);

	if(btn==0){	// no buttons pressed
		audio.event = 0;
		btn_prev = btn;
	}

	if((btn^btn_prev)){	// button activity changed
		u8 btnCh = 0;
		btnCh = btn^btn_prev;

		if(btnCh&BTNL_MASK){	// BTNL: record
			audio.btnchar = 'l';
			audio.record = 1;
			xil_printf("	BTNL \r\n");
		}
		if(btnCh&BTNC_MASK){	// BTNC: playback
			audio.btnchar = 'c';
			audio.playback = 1;
			xil_printf("	BTNL \r\n");
		}
		audio.event = 1;
		btn_prev = btn;
	}
}

//----- interrupt stuff
unsigned char InterruptConfig(){
	int Status;

	// initialize GPIO driver for buttons & switches
	Status = XGpio_Initialize(&Gpio, BUTTON_SWITCH_ID);
	if(Status != XST_SUCCESS){return XST_FAILURE;}

	// set buttons & switches directions to outputs
	XGpio_SetDataDirection(&Gpio, BUTTON_CHANNEL, 0xFF);
	XGpio_SetDataDirection(&Gpio, SWITCH_CHANNEL, 0xFF);

	// initialize interrupt controller
	Status = IntcInitFunction(INTC_DEVICE_ID, &Gpio);
	if(Status != XST_SUCCESS){return XST_FAILURE;}

	Xil_AssertWait = 0;	// ?
	return XST_SUCCESS;
}

int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr){
	XScuGic_Config *IntcConfig;
	int Status;

	// interrupt controller initialization
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	Status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if(Status != XST_SUCCESS){return XST_FAILURE;}

	// call to interrupt setup
	Status = InterruptSystemSetup(&INTCInst);
	if(Status != XST_SUCCESS){return XST_FAILURE;}

	// connect GPIO interrupt to handler
	Status = XScuGic_Connect(	&INTCInst,
								INTC_GPIO_INTERRUPT_ID,
								(Xil_ExceptionHandler)BTN_Intr_Handler,
								(void*)GpioInstancePtr	);
	if(Status != XST_SUCCESS){return XST_FAILURE;}

	// enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, BUTTON_CHANNEL);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// enable GPio interrupts in the controller
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

	return XST_SUCCESS;
}

void BTN_Intr_Handler(void *InstancePtr){
	// disable GPIO interrupts
	XGpio_InterruptDisable(&Gpio, BUTTON_CHANNEL);

	// ignore additional button presses
	if(((XGpio_InterruptGetStatus(&Gpio)&BUTTON_CHANNEL)
			&BUTTON_CHANNEL)!=BUTTON_CHANNEL){return;}

	// get button data
	BTN_Event();
	xil_printf("Button Interrupt Received \r\n");

	(void)XGpio_InterruptClear(&Gpio, BUTTON_CHANNEL);

	// enable GPIO interrupts
	XGpio_InterruptEnable(&Gpio, BUTTON_CHANNEL);
}

int InterruptSystemSetup(XScuGic *XScuGicInstancePtr){
	// enable interrupt
	XGpio_InterruptEnable(&Gpio, BUTTON_CHANNEL);
	XGpio_InterruptGlobalEnable(&Gpio);

	Xil_ExceptionRegisterHandler(	XIL_EXCEPTION_ID_INT,
									(Xil_ExceptionHandler)XScuGic_InterruptHandler,
									XScuGicInstancePtr	);
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}
