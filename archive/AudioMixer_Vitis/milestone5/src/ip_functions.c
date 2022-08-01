/*
 * ip_functions.c
 *
 * Contains all functions which pertain to setup and use of IP periperals.
 */

#include "adventures_with_ip.h"
#include <math.h>

XGpio BTNInst;
XGpio SWSInst;
XScuGic INTCInst;
int btn_value;

u32 recL[NUM_AUDIO_SAMPLES];
u32 recR[NUM_AUDIO_SAMPLES];

//-----prototype functions
static void BTN_Intr_Handler(void *baseaddr_p);
static int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr);
//-----

//u32 leftRecord[50000000], rightRecord[50000000];
int i=0;


void audio_stream2(){

	u32 data_raw;
	u32 data_energy;
	u32 data_in;

	u32 in_left, in_right;
	//u32 leftval, rightval;

	int valuefromswitches, prevswitchvalue;
	int valuefrombuttons, prevbuttonvalue;
	int debounce = 0;

	u32 energy, rms;
	//double valL = 0;
	//double valR = 0;

	short Track1Bool = 0;
	short Track2Bool = 0;
	short Track3Bool = 0;
	short Track4Bool = 0;

	int index = 0;
//	int MaxValue = 500000;
//	int MaxValue = 4000000;
	int MaxValue = 1000000;

	u32 Track1RightData[MaxValue];
	u32 Track1LeftData[MaxValue];
	u32 Track2RightData[MaxValue];
	u32 Track2LeftData[MaxValue];
	u32 Track3RightData[MaxValue];
	u32 Track3LeftData[MaxValue];
	u32 Track4RightData[MaxValue];
	u32 Track4LeftData[MaxValue];


	u32 AddTrack1Right[MaxValue];
	u32 AddTrack1Left[MaxValue];
	u32 AddTrack2Right[MaxValue];
	u32 AddTrack2Left[MaxValue];
	u32 ResultRight[MaxValue];
	u32 ResultLeft[MaxValue];

	short Recording=0;
	short Playback=0;
	short AddAudio=0;
    short ReverseAudio=0;
    short CombineAudio=0;
    short LiveAudio=0; //

	while (!XUartPs_IsReceiveData(UART_BASEADDR)){

		valuefromswitches = XGpio_DiscreteRead(&Gpio, SWITCH_CHANNEL);
		if (valuefromswitches != prevswitchvalue){
			xil_printf("Switches changed: ");

			if ((valuefromswitches & 128) == 128){
				xil_printf(" SW7 ");
				Track1Bool=1;
			} else { Track1Bool=0; }

			if ((valuefromswitches & 64) == 64){
				xil_printf(" SW6 ");
				Track2Bool=1;
			} else { Track2Bool=0; }

			if ((valuefromswitches & 32) == 32){
				xil_printf(" SW5 ");
				Track3Bool=1;
				} else { Track3Bool=0; }

			if ((valuefromswitches & 16) == 16){
				xil_printf(" SW4 ");
				Track4Bool=1;
			} else { Track4Bool=0; }

			if ((valuefromswitches & 8) == 8){
				xil_printf(" SW3 ");
			}
			if ((valuefromswitches & 4) == 4){
				xil_printf(" SW2 ");
			}
			if ((valuefromswitches & 2) == 2){
				xil_printf(" SW1 ");
			}
			if ((valuefromswitches & 1) == 1){
				xil_printf(" SW0 ");
			}
			if ((valuefromswitches != 0)){
				xil_printf("Now Active \r \n");
			}
			if ((valuefromswitches == 0)){
				xil_printf("No Switches Active \r\n");
			}
			xil_printf(" \r\n");
		}

		valuefrombuttons = XGpio_DiscreteRead(&Gpio, BUTTON_CHANNEL);
		if((valuefrombuttons != prevbuttonvalue) && (debounce==0)){
			debounce=125000;
			if (valuefrombuttons != 0) xil_printf("Button Pressed: ");
			if ((valuefrombuttons & 16) == 16){
				xil_printf(" BTNU");
				//AddAudio=1;
				LiveAudio=1;
			}
			else {
				LiveAudio=0;
				index=0;
				//AddAudio=0;
			}
			if ((valuefrombuttons & 8) == 8){
				xil_printf(" BTNR ");
				ReverseAudio=1;
			}
			if ((valuefrombuttons & 4) == 4){
				xil_printf(" BTNL ");
				Recording=1;
			}
			else {
				Recording=0;
				index=0;
			}
			if ((valuefrombuttons & 2) == 2){
			    xil_printf(" BTND ");
			    CombineAudio=1;
			}
			else {
			    CombineAudio=0;
			    index=0;
			}
			if ((valuefrombuttons & 1) == 1){
				xil_printf(" BTNC ");
				Playback=1;
			}
			else{
				Playback=0;
				index=0;
			}
			xil_printf(" \r\n");
			if (valuefrombuttons != 0) xil_printf("\r \n ");
			if (valuefrombuttons == 0) xil_printf("All Buttons Released \r \n ");
		}

		// Read audio input from codec
		in_left = Xil_In32(I2S_DATA_RX_L_REG);
		in_right = Xil_In32(I2S_DATA_RX_R_REG);


		if(Recording==1){
			if(Track1Bool==1){
				Track1RightData[index]=in_right;
				Track1LeftData[index]=in_left;
			}
			else if (Track2Bool==1){
				Track2RightData[index]=in_right;
				Track2LeftData[index]=in_left;
			}
			else if (Track3Bool==1){
				Track3RightData[index]=in_right;
				Track3LeftData[index]=in_left;
			}
			else if (Track4Bool==1){
				Track4RightData[index]=in_right;
				Track4LeftData[index]=in_left;
			}
			else{}

			if (index < MaxValue){ index++; }
			else{ index=0; }
		}

		// Write audio output to codec

		if(Playback==1){
			if(Track1Bool==1){
				Xil_Out32(I2S_DATA_TX_L_REG, Track1RightData[index]);
				Xil_Out32(I2S_DATA_TX_R_REG, Track1LeftData[index]);
			}
			else if (Track2Bool==1){
				Xil_Out32(I2S_DATA_TX_L_REG, Track2RightData[index]);
				Xil_Out32(I2S_DATA_TX_R_REG, Track2LeftData[index]);
			}
			else if (Track3Bool==1){
				Xil_Out32(I2S_DATA_TX_L_REG, Track3RightData[index]);
				Xil_Out32(I2S_DATA_TX_R_REG, Track3LeftData[index]);
			}
			else if (Track4Bool==1){
				Xil_Out32(I2S_DATA_TX_L_REG, Track4RightData[index]);
				Xil_Out32(I2S_DATA_TX_R_REG, Track4LeftData[index]);
			}
			else{ }

			if (index < MaxValue){ index++; }
			else{ index=0; }
		}
		else{

			data_raw = XAdcPs_GetAdcData(&XAdcInst, XADCPS_CH_VCCAUX);
			data_energy = data_raw << 1;
			data_in  = in_left >> 6;
//			mindata = XAdcPs_GetMinMaxMeasurement(&XAdcInst, XADCPS_MIN_VCCAUX);
//			maxdata = XAdcPs_GetMinMaxMeasurement(&XAdcInst, XADCPS_MAX_VCCAUX);
			xil_printf("raw data = %d\t, energy = %d\t, data in = %d\r\n", data_raw, data_energy, data_in);

			//xil_printf("data = %d\r\n", data_in);

			Xil_Out32(I2S_DATA_TX_L_REG, in_left);
			Xil_Out32(I2S_DATA_TX_R_REG, in_right);


		}

//-----Live Stuff
		u32 live_left;
		u32 live_right;


		u32 liveDataL[MaxValue];
		u32 liveDataR[MaxValue];
		if(LiveAudio==1){
			live_left = Xil_In32(I2S_DATA_RX_L_REG);
			live_right = Xil_In32(I2S_DATA_RX_R_REG);

			liveDataL[index]=live_left;
			liveDataR[index]=live_right;
			if(Track1Bool==1){
				Track1RightData[index] = Track1RightData[index]+liveDataR[index];
				Track1LeftData[index] = Track1LeftData[index]+liveDataL[index];
			}
			if(Track2Bool==1){
				Track2RightData[index] = Track2RightData[index]+liveDataR[index];
				Track2LeftData[index] = Track2LeftData[index]+liveDataL[index];
			}
			if(Track3Bool==1){
				Track3RightData[index] = Track3RightData[index]+liveDataR[index];
				Track3LeftData[index] = Track3LeftData[index]+liveDataL[index];
			}
			if(Track4Bool==1){
				Track4RightData[index] = Track4RightData[index]+liveDataR[index];
				Track4LeftData[index] = Track4LeftData[index]+liveDataL[index];
			}
			if(index<MaxValue){index++;} else{index=0;}

		}
//-----

		if(AddAudio==1){
			if((Track1Bool==1)&&(Track2Bool==1)){
				for(i=0; i<MaxValue; i++){
					AddTrack1Right[i]=Track1RightData[i];
					AddTrack1Left[i]=Track1LeftData[i];
					AddTrack2Right[i]=Track2RightData[i];
					AddTrack2Left[i]=Track2LeftData[i];
				}
			}
			else if((Track1Bool==1)&&(Track3Bool==1)){
				for(i=0; i<MaxValue; i++){
					AddTrack1Right[i]=Track1RightData[i];
					AddTrack1Left[i]=Track1LeftData[i];
					AddTrack2Right[i]=Track3RightData[i];
					AddTrack2Left[i]=Track3LeftData[i];
				}
			}
			else if((Track1Bool==1)&&(Track4Bool==1)){
				for(i=0; i<MaxValue; i++){
					AddTrack1Right[i]=Track1RightData[i];
					AddTrack1Left[i]=Track1LeftData[i];
					AddTrack2Right[i]=Track4RightData[i];
					AddTrack2Left[i]=Track4LeftData[i];
				}

			}
			else if((Track2Bool==1)&&(Track3Bool==1)){
				for(i=0; i<MaxValue; i++){
					AddTrack1Right[i]=Track2RightData[i];
					AddTrack1Left[i]=Track2LeftData[i];
					AddTrack2Right[i]=Track3RightData[i];
					AddTrack2Left[i]=Track3LeftData[i];
				}
			}
			else if((Track2Bool==1)&&(Track4Bool==1)){
				for(i=0; i<MaxValue; i++){
					AddTrack1Right[i]=Track2RightData[i];
					AddTrack1Left[i]=Track2LeftData[i];
					AddTrack2Right[i]=Track4RightData[i];
					AddTrack2Left[i]=Track4LeftData[i];
				}
			}
			else if((Track3Bool==1)&&(Track4Bool==1)){
				for(i=0; i<MaxValue; i++){
					AddTrack1Right[i]=Track3RightData[i];
					AddTrack1Left[i]=Track3LeftData[i];
					AddTrack2Right[i]=Track4RightData[i];
					AddTrack2Left[i]=Track4LeftData[i];
				}
			}

			for(i=0;i<MaxValue;i++){
				if(AddTrack1Right[i] > AddTrack2Right[i]){
					ResultRight[i]=AddTrack1Right[i];
				}
				else{
					ResultRight[i]=AddTrack2Right[i];
				}
				if(AddTrack1Left[i] > AddTrack2Left[i]){
					ResultLeft[i]=AddTrack1Left[i];
				}
				else{
					ResultLeft[i]=AddTrack2Left[i];
				}
			}

			if(Track1Bool==1){
				for(i=0; i<MaxValue; i++){
					Track1RightData[i]=ResultRight[i];
					Track1LeftData[i]=ResultLeft[i];
				}
			}
			else if(Track2Bool==1){
				for(i=0; i<MaxValue; i++){
					Track2RightData[i]=ResultRight[i];
					Track2LeftData[i]=ResultLeft[i];
				}
			}
			else if(Track3Bool==1){
				for(i=0; i<MaxValue; i++){
					Track3RightData[i]=ResultRight[i];
					Track3LeftData[i]=ResultLeft[i];
				}
			}
			//Stay in the adding loop until add Audio button is removed.
			while((XGpio_DiscreteRead(&Gpio, BUTTON_CHANNEL) & 16) == 16){}
			AddAudio=0;
		}

//------Reverse Audio
        if(ReverseAudio==1){
            short TrackReversedBool=0;
            xil_printf("Reversing Selected Tracks: ");
            if(Track1Bool==1){
                TrackReversedBool=1;
                xil_printf("Track 1");
                u32 TempRightData[MaxValue];
                u32 TempLeftData[MaxValue];

                for(i=0; i<MaxValue; i++){
                    //TrackReversedBool=1;
                    TempRightData[i]=Track1RightData[(MaxValue-i)];
                    TempLeftData[i]=Track1LeftData[(MaxValue-i)];
                }
                for(i=0; i<MaxValue; i++){
                    Track1RightData[i]=TempRightData[i];
					Track1LeftData[i]=TempLeftData[i];
                }
            }
            if(Track2Bool==1){
                TrackReversedBool=1;
                xil_printf("Track 2");
                u32 TempRightData[MaxValue];
                u32 TempLeftData[MaxValue];

                for(i=0; i<MaxValue; i++){
                    TempRightData[i]=Track1RightData[(MaxValue-i)];
                    TempLeftData[i]=Track1LeftData[(MaxValue-i)];
                }
                for(i=0; i<MaxValue; i++){
                    Track2RightData[i]=TempRightData[i];
					Track2LeftData[i]=TempLeftData[i];
                }
            }
            if(Track3Bool==1){
                TrackReversedBool=1;
                xil_printf("Track 3");
                u32 TempRightData[MaxValue];
                u32 TempLeftData[MaxValue];

                for(i=0; i<MaxValue; i++){
                    TempRightData[i]=Track1RightData[(MaxValue-i)];
                    TempLeftData[i]=Track1LeftData[(MaxValue-i)];
                }
                for(i=0; i<MaxValue; i++){
                    Track3RightData[i]=TempRightData[i];
					Track3LeftData[i]=TempLeftData[i];
                }
            }
            if(Track4Bool==1){
                TrackReversedBool=1;
                xil_printf("Track 4");
                u32 TempRightData[MaxValue];
                u32 TempLeftData[MaxValue];

                for(i=0; i<MaxValue; i++){
                    TempRightData[i]=Track1RightData[(MaxValue-i)];
                    TempLeftData[i]=Track1LeftData[(MaxValue-i)];
                }
                for(i=0; i<MaxValue; i++){
                    Track4RightData[i]=TempRightData[i];
					Track4LeftData[i]=TempLeftData[i];
                }
            }
            if(TrackReversedBool==0){
                xil_printf("No Tracks Selected: No Tracks Reversed");
            }
            xil_printf(" \r\n");
            ReverseAudio=0;
        }

//------Combine Audio
        if(CombineAudio==1){
            short TrackCombinedBool=0;

            // Check if more than two tracks are selected
            // Combined audio saved to lowest Track#
            if(Track1Bool+Track2Bool+Track3Bool+Track4Bool < 2){
                xil_printf("Not Enough Tracks Selected \r\n");
            }
            else{
            	if(Track1Bool+Track2Bool+Track3Bool+Track4Bool == 2){
            		xil_printf("Combining Selected Tracks: ");

            		if(Track1Bool && Track2Bool){
            			TrackCombinedBool=1;
            		    xil_printf("Track 1, Track 2 \r\n");

            		    for(i=0; i<MaxValue; i++){
            		    	Track1RightData[i]=Track1RightData[i]+Track2RightData[i];
            		        Track1LeftData[i]=Track1LeftData[i]+Track2LeftData[i];
            		    }
            		}
            		if(Track1Bool && Track3Bool){
            			TrackCombinedBool=1;
            		    xil_printf("Track 1, Track 3 \r\n");

            		    for(i=0; i<MaxValue; i++){
            		    	Track1RightData[i]=Track1RightData[i]+Track3RightData[i];
            		        Track1LeftData[i]=Track1LeftData[i]+Track3LeftData[i];
            		    }
            		}
            		if(Track1Bool && Track4Bool){
            			TrackCombinedBool=1;
            		    xil_printf("Track 1, Track 4 \r\n");

            		    for(i=0; i<MaxValue; i++){
            		    	Track1RightData[i]=Track1RightData[i]+Track4RightData[i];
            		        Track1LeftData[i]=Track1LeftData[i]+Track4LeftData[i];
            		    }
            		}
            		if(Track2Bool && Track3Bool){
            			TrackCombinedBool=1;
            		    xil_printf("Track 2, Track 3 \r\n");

            		    for(i=0; i<MaxValue; i++){
            		    	Track2RightData[i]=Track2RightData[i]+Track3RightData[i];
            		        Track2LeftData[i]=Track2LeftData[i]+Track3LeftData[i];
            		    }
            		}
            		if(Track2Bool && Track4Bool){
            			TrackCombinedBool=1;
            		    xil_printf("Track 2, Track 4 \r\n");

            		    for(i=0; i<MaxValue; i++){
            		    	Track2RightData[i]=Track2RightData[i]+Track4RightData[i];
            		        Track2LeftData[i]=Track2LeftData[i]+Track4LeftData[i];
            		    }
            		}
            		if(Track3Bool && Track4Bool){
            			TrackCombinedBool=1;
            		    xil_printf("Track 3, Track 4 \r\n");

            		    for(i=0; i<MaxValue; i++){
            		    	Track3RightData[i]=Track3RightData[i]+Track4RightData[i];
            		        Track3LeftData[i]=Track3LeftData[i]+Track4LeftData[i];
            		    }
            		}
            	}
            	if(Track1Bool+Track2Bool+Track3Bool+Track4Bool == 3){
            		xil_printf("Combining Selected Tracks: ");
            		u32 TempRightData[MaxValue];
            		u32 TempLeftData[MaxValue];

            		if(Track1Bool && Track2Bool && Track3Bool){
            			TrackCombinedBool=1;
            			xil_printf("Track 1, Track 2, Track 3 \r\n");

            			for(i=0; i<MaxValue; i++){
            				TempRightData[i]=Track1RightData[i]+Track2RightData[i];
            				TempLeftData[i]=Track1LeftData[i]+Track2LeftData[i];
            			}
            			for(i=0; i<MaxValue; i++){
            				Track1RightData[i]=TempRightData[i]+Track3RightData[i];
            				Track1LeftData[i]=TempLeftData[i]+Track3LeftData[i];
            			}
            		}
            		if(Track1Bool && Track2Bool && Track4Bool){
            			TrackCombinedBool=1;
            			xil_printf("Track 1, Track 2, Track 4 \r\n");

            			for(i=0; i<MaxValue; i++){
            				TempRightData[i]=Track1RightData[i]+Track2RightData[i];
            				TempLeftData[i]=Track1LeftData[i]+Track2LeftData[i];
            			}
            			for(i=0; i<MaxValue; i++){
            				Track1RightData[i]=TempRightData[i]+Track4RightData[i];
            				Track1LeftData[i]=TempLeftData[i]+Track4LeftData[i];
            			}
            		}
            		if(Track1Bool && Track3Bool && Track4Bool){
            			TrackCombinedBool=1;
            			xil_printf("Track 1, Track 3, Track 4 \r\n");

            			for(i=0; i<MaxValue; i++){
            				TempRightData[i]=Track1RightData[i]+Track3RightData[i];
            				TempLeftData[i]=Track1LeftData[i]+Track3LeftData[i];
            			}
            			for(i=0; i<MaxValue; i++){
            				Track1RightData[i]=TempRightData[i]+Track4RightData[i];
            				Track1LeftData[i]=TempLeftData[i]+Track4LeftData[i];
            			}
            		}
            		if(Track2Bool && Track3Bool && Track4Bool){
            			TrackCombinedBool=1;
            			xil_printf("Track 2, Track 3, Track 4 \r\n");

            			for(i=0; i<MaxValue; i++){
            				TempRightData[i]=Track2RightData[i]+Track3RightData[i];
            				TempLeftData[i]=Track2LeftData[i]+Track3LeftData[i];
            			}
            			for(i=0; i<MaxValue; i++){
            				Track2RightData[i]=TempRightData[i]+Track4RightData[i];
            				Track2LeftData[i]=TempLeftData[i]+Track4LeftData[i];
            			}
            		}
            	}
            	if(Track1Bool+Track2Bool+Track3Bool+Track4Bool == 4){
            		xil_printf("Combining Selected Tracks: ");
            		u32 Temp1RightData[MaxValue];
            		u32 Temp1LeftData[MaxValue];
            		u32 Temp2RightData[MaxValue];
            		u32 Temp2LeftData[MaxValue];

            		if(Track2Bool && Track3Bool && Track4Bool){
            			TrackCombinedBool=1;
            			xil_printf("Track 1, Track 2, Track 3, Track 4 \r\n");

            			for(i=0; i<MaxValue; i++){
            				Temp1RightData[i]=Track1RightData[i]+Track2RightData[i];
            				Temp1LeftData[i]=Track1LeftData[i]+Track2LeftData[i];
            			}
            			for(i=0; i<MaxValue; i++){
            				Temp2RightData[i]=Temp1RightData[i]+Track3RightData[i];
            				Temp2LeftData[i]=Temp1LeftData[i]+Track3LeftData[i];
            			}
            			for(i=0; i<MaxValue; i++){
            				Track1RightData[i]=Temp2RightData[i]+Track4RightData[i];
            				Track1LeftData[i]=Temp2LeftData[i]+Track4LeftData[i];
            			}
            		}

            	}
            }
            if(TrackCombinedBool==0){
                xil_printf("Not Enough Tracks Selected: No Tracks Combined \r\n");
            }
            xil_printf(" \r\n");
            xil_printf("Audio Combine Complete \r\n");
            xil_printf(" \r\n");
            CombineAudio=0;
        }
//------
		prevswitchvalue=valuefromswitches;
		prevbuttonvalue=valuefrombuttons;

		if(debounce > 0) debounce--;
	}
}

/* ---------------------------------------------------------------------------- *
 * 								gpio_init()								    	*
 * ---------------------------------------------------------------------------- *
 * Initializes the GPIO driver for the push buttons and switches.
 * ---------------------------------------------------------------------------- */
unsigned char gpio_init()
{
	int Status;

	Status = XGpio_Initialize(&Gpio, BUTTON_SWITCH_ID);
	if(Status != XST_SUCCESS) return XST_FAILURE;

	XGpio_SetDataDirection(&Gpio, SWITCH_CHANNEL, 0xFF);
	XGpio_SetDataDirection(&Gpio, BUTTON_CHANNEL, 0xFF);

	return XST_SUCCESS;
}


//------Interrupt Stuff
//------
unsigned char irpt_init()
{
	int Status;

	// btns init
	Status = XGpio_Initialize(&BTNInst, BUTTON_SWITCH_ID);
	if(Status != XST_SUCCESS) return XST_FAILURE;
	XGpio_SetDataDirection(&BTNInst, BUTTON_CHANNEL, 0xFF);

	// sws init
	Status = XGpio_Initialize(&SWSInst, BUTTON_SWITCH_ID);
	if(Status != XST_SUCCESS) return XST_FAILURE;
	XGpio_SetDataDirection(&SWSInst, BUTTON_CHANNEL, 0xFF);

	// Initialize interrupt controller
	Status = IntcInitFunction(INTC_DEVICE_ID, &BTNInst);
	if(Status != XST_SUCCESS) return XST_FAILURE;

	return XST_SUCCESS;
}

// From Zynq_Book_Tutorial_2B
// modified
int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr)
{
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialisation
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Call to interrupt setup
	status = InterruptSystemSetup(&INTCInst);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&INTCInst,
					  	  	 INTC_GPIO_IRPT_ID,
					  	  	 (Xil_ExceptionHandler)BTN_Intr_Handler,
					  	  	 (void *)GpioInstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// Enable GPIO and timer interrupts in the controller
	XScuGic_Enable(&INTCInst, INTC_GPIO_IRPT_ID);

	return XST_SUCCESS;
}

// From Zynq_Book_Tutorial_2B
// modified
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
	// Enable interrupt
	XGpio_InterruptEnable(&BTNInst, BUTTON_CHANNEL);
	XGpio_InterruptGlobalEnable(&BTNInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 	 	 	 	 	 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
			 	 	 	 	 	 XScuGicInstancePtr);
	Xil_ExceptionEnable();


	return XST_SUCCESS;
}

// From Zynq_Book_Tutorial_2B
// modified
void BTN_Intr_Handler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&BTNInst, BUTTON_CHANNEL);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&BTNInst) & BUTTON_CHANNEL) !=
			BUTTON_CHANNEL) {
			return;
		}
	btn_value = XGpio_DiscreteRead(&BTNInst, 1);

	// Increment counter based on button value
	// Reset if center button pressed
	if(btn_value != 1){ btn_value = btn_value + btn_value; }
	else{ btn_value = 0; }
	xil_printf("Interrupt Received, value %d \n \r", btn_value);

 //   XGpio_DiscreteWrite(&LEDInst, 1, led_data);

    (void)XGpio_InterruptClear(&BTNInst, BUTTON_CHANNEL);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(&BTNInst, BUTTON_CHANNEL);
}
//------Interrupt stuff end


// adc stuff
unsigned char xadc_init(){
	XAdcPs_Config *ConfigPtr;
	ConfigPtr =XAdcPs_LookupConfig(XPAR_PS7_XADC_0_DEVICE_ID);
	if(ConfigPtr==NULL){return XST_FAILURE;}

	int Status;
	Status = XAdcPs_CfgInitialize(&XAdcInst, ConfigPtr, ConfigPtr->BaseAddress);
	if(Status!=XST_SUCCESS){return XST_FAILURE;}

	XAdcPs_SetSequencerMode(&XAdcInst, XADCPS_SEQ_MODE_SAFE); //XADCOS_SEQ_MODE_CONTINPASS?

	return XST_SUCCESS;
}

