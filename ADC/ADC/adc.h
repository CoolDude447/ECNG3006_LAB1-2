#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include "app.h"

// Function Prototypes

/**
 * \brief Initializes the ADC module.
 * \return true if initialization is successful, false otherwise.
 */
bool init_adc(void);

/**
 * \brief Starts an ADC conversion.
 * \return true if start is successful, false otherwise.
 */
bool start_adc(void);

/**
 * \brief Stops the ADC conversion.
 * \return true if stop is successful, false otherwise.
 */
bool stop_adc(void);

/**
 * \brief Checks if the ADC conversion is complete.
 * \return true if conversion is complete, false otherwise.
 */
bool is_done_adc(void);

/**
 * \brief Reads a single conversion result from the ADC.
 * \param channel The ADC channel number to read.
 * \return The ADC conversion result.
 */
int32_t singleread_adc(uint8_t channel);

#endif /* ADC_H_ */
