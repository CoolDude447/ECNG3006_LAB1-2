//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "app.h"
#include "clock.h"
#include <stdio.h>
#include <stdlib.h>

#define F_CPU 48000000UL
#include "delay.h"

#include "adc.h"
#include "USART3.h"

/*******************************************************************************
 * Function:        void AppInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine takes care of all of the 1-time hardware/software
 *                  initialization that is required.
 *
 * Note:            This was taken from the _00_LED_ON provided by
 *                  Microchip Technology Inc. 
 *
 ******************************************************************************/
void AppInit(void)
{
	/*	Clock initialization (CPU, AHB, APBx, Asynchronous Peripheral Clocks)
		The System RC Oscillator (RCSYS) provides the source for the main clock
		at chip startup. It is set to 1MHz.
	*/
	ClocksInit();
	
	// Assign LED0 as OUTPUT
	REG_PORT_DIR0 = LED0_PIN_MASK;
	
	// Set LED0 OFF
	REG_PORT_OUTCLR0 = LED0_PIN_MASK;
	

} // AppInit()

/*******************************************************************************
 * Function:        void AppRun(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function contains your main application
 *                  
 *
 * Note:
 *
 ******************************************************************************/
void AppRun(void)
{
	// Initialize the UART at 9600 baud
	UART3_Init(9600);
	delay_ms(500);
	UART3_Write_Text("UART Initialized successfully at 9600 baud.\r\n");

	// Initialize the ADC
	if (init_adc()) {
		UART3_Write_Text("ADC Initialized successfully.\r\n");
		} else {
		UART3_Write_Text("ADC Initialization failed.\r\n");
		return;  // Exit if initialization fails
	}
	delay_ms(100);

	// Variable to store the ADC result
	int32_t adc_result;
	char buffer[10];  // Buffer to hold the ADC result as a string

	// Test loop for reading ADC values
	while (1)
	{
		// Start ADC conversion
		if (start_adc()) {
			UART3_Write_Text("ADC Conversion Started.\r\n");
			} else {
			UART3_Write_Text("ADC Conversion Start Failed.\r\n");
			break;
		}

		// Wait for conversion to complete
		while (!is_done_adc());

		// Read the ADC result from channel 19 (PA11, connected to A0)
		adc_result = singleread_adc(19);

		// Convert ADC result to string
		itoa(adc_result, buffer, 10);

		// Send ADC reading over UART
		UART3_Write_Text("ADC Reading: ");
		UART3_Write_Text(buffer);
		UART3_Write_Text("\r\n");

		// Stop the ADC to demonstrate stop functionality
		if (stop_adc()) {
			UART3_Write_Text("ADC Conversion Stopped.\r\n");
			} else {
			UART3_Write_Text("ADC Conversion Stop Failed.\r\n");
		}

		// Delay between readings
		delay_ms(10000);
	}
}




