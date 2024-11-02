#include "adc.h"

/*******************************************************************************
 * Function:        bool init_adc(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          true if initialization is successful, false otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        Initializes the ADC module, setting up the clock, calibration,
 *                  reference voltage, sample length, and enabling the ADC.
 *
 * Note:            Default initialization is set to 10-bit resolution.
 ******************************************************************************/
bool init_adc(void)
{
    // Enable APBC clock for ADC
    REG_PM_APBCMASK |= PM_APBCMASK_ADC;
    while (!(REG_PM_APBCMASK & PM_APBCMASK_ADC));

    // Assign clock source to ADC
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_ADC | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY);

    // Reset ADC
    ADC->CTRLA.reg = ADC_CTRLA_SWRST;
    while (ADC->CTRLA.bit.SWRST || ADC->STATUS.bit.SYNCBUSY);

    // Load calibration data
    ADC->CALIB.reg = ADC_CALIB_BIAS_CAL((*(uint32_t *)ADC_FUSES_BIASCAL_ADDR >> ADC_FUSES_BIASCAL_Pos)) |
                     ADC_CALIB_LINEARITY_CAL((*(uint64_t *)ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos));

    // Set reference to VCC (3.3V)
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC0;

    // Set sample length and prescaler
    ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(10);
    ADC->CTRLB.bit.PRESCALER = ADC_CTRLB_PRESCALER_DIV32_Val;

    // Enable ADC
    ADC->CTRLA.reg = ADC_CTRLA_ENABLE;
    while (ADC->STATUS.bit.SYNCBUSY);

    return ADC->CTRLA.bit.ENABLE;
}

/*******************************************************************************
 * Function:        bool start_adc(void)
 *
 * PreCondition:    ADC module must be initialized and enabled.
 *
 * Input:           None
 *
 * Output:          true if start is successful, false otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        Starts an ADC conversion.
 *
 * Note:            Ensure ADC is configured before calling.
 ******************************************************************************/
bool start_adc(void)
{
    // Trigger an ADC start conversion
    ADC->SWTRIG.reg = ADC_SWTRIG_START;
    return true;
}

/*******************************************************************************
 * Function:        bool stop_adc(void)
 *
 * PreCondition:    ADC module must be initialized and running.
 *
 * Input:           None
 *
 * Output:          true if stop is successful, false otherwise.
 *
 * Side Effects:    ADC conversion is stopped.
 *
 * Overview:        Stops an ongoing ADC conversion.
 *
 * Note:            Ensure ADC is actively converting before calling.
 ******************************************************************************/
bool stop_adc(void)
{
    // Disable ADC to stop conversion
    ADC->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
    return !ADC->CTRLA.bit.ENABLE;
}

/*******************************************************************************
 * Function:        bool is_done_adc(void)
 *
 * PreCondition:    ADC module must be initialized and a conversion started.
 *
 * Input:           None
 *
 * Output:          true if conversion is complete, false otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        Checks if the ADC conversion is complete.
 *
 * Note:            None
 ******************************************************************************/
bool is_done_adc(void)
{
    // Check if conversion is complete
    return ADC->INTFLAG.bit.RESRDY;
}

/*******************************************************************************
 * Function:        int32_t singleread_adc(uint8_t channel)
 *
 * PreCondition:    ADC module must be initialized.
 *
 * Input:           channel - The ADC channel number to read.
 *
 * Output:          The conversion result from the specified ADC channel.
 *
 * Side Effects:    None
 *
 * Overview:        Configures the ADC to read a specified channel, triggers a 
 *                  conversion, and waits for the result before returning it.
 *
 * Note:            Ensure the channel is valid for the ADC.
 ******************************************************************************/
int32_t singleread_adc(uint8_t channel)
{
    // Set MUX to specified channel
    ADC->INPUTCTRL.bit.MUXPOS = channel;
    while (ADC->STATUS.bit.SYNCBUSY);

    // Start conversion and flush previous result
    start_adc();
    ADC->SWTRIG.reg |= ADC_SWTRIG_FLUSH;

    // Wait for conversion to complete
    while (!is_done_adc());

    // Return the conversion result
    return ADC->RESULT.reg;
}
