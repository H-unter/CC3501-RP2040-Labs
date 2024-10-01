#include "microphone.h"
#include <stdio.h>

// Constructor
microphone::microphone()
    : gpio_pin(26) {}

/*! \brief Initialize the microphone by setting up the ADC.
 *
 * This method configures the GPIO for microphone input (GPIO26 by default),
 * and sets up the ADC in free-running mode.
 *
 * \param gpio_pin The GPIO pin to read microphone data from (default: GPIO26).
 */
void microphone::init(uint gpio_pin)
{
    this->gpio_pin = gpio_pin;

    // Initialize GPIO for analogue use
    adc_gpio_init(this->gpio_pin);

    // Initialize ADC
    adc_init();
    adc_select_input(0); // Channel 0 corresponds to GPIO26

    // Set the ADC clock divider to 0 for maximum speed
    adc_set_clkdiv(1087);

    // Enable the ADC FIFO (without DMA)
    adc_fifo_setup(
        true,  // Write each completed conversion to the sample FIFO
        false, // Disable DMA data request (DREQ)
        1,     // Trigger when at least 1 sample is present
        false, // Disable error bits
        false  // Shift results to 8 bits
    );

    // Start ADC
    adc_run(true);
    printf("Microphone initialized on GPIO pin %d\n", this->gpio_pin);
}

/*! \brief Blocking read of ADC samples.
 *
 * This function reads samples from the ADC and stores them in the provided buffer.
 * It enables the ADC in free-running mode and blocks until all the requested samples
 * are read into the buffer. After reading the specified number of samples, the function
 * stops the ADC from free-running mode and drains the FIFO buffer to discard any
 * remaining samples that were collected by the ADC but not yet processed.
 *
 * \param buffer Pointer to the buffer to store ADC samples.
 * \param buffer_size The size of the buffer (i.e., the number of samples to read).
 *
 * \note This function drains the FIFO after the required number of samples are read,
 *       ensuring no leftover data remains in the FIFO.
 */
void microphone::read_blocking(uint16_t *buffer, size_t buffer_size)
{
    adc_run(true); // Enable free-running mode

    for (size_t i = 0; i < buffer_size; ++i)
    {
        buffer[i] = adc_fifo_get_blocking(); // Block until sample is available
    }

    adc_run(false);   // Stop free-running mode after reading required samples
    adc_fifo_drain(); // Drain any leftover samples in the FIFO
}
