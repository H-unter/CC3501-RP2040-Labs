#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "hardware/adc.h"
#include "pico/stdlib.h"

/*! \brief A class to handle microphone input using the ADC on the RP2040.
 *
 * This class provides methods to initialize the ADC and sample data from the microphone.
 */
class microphone
{
public:
    // Constructor
    microphone();

    /*! \brief Initialize the microphone by setting up the ADC.
     *
     * This method configures the GPIO for microphone input (GPIO26 by default),
     * and sets up the ADC in free-running mode.
     *
     * \param gpio_pin The GPIO pin to read microphone data from (default: GPIO26).
     */
    void init(uint gpio_pin = 26);

    /*! \brief Blocking read of ADC samples.
     *
     * This function reads samples from the ADC and stores them in a provided buffer.
     *
     * \param microphone_data Pointer to the buffer to store ADC samples.
     * \param buffer_size The size of the buffer.
     */
    void read_blocking(int16_t *microphone_data, size_t buffer_size);

    /*! \brief Remove DC offset and scale the microphone data.
     *
     * This function removes the DC offset from the microphone data and scales it by performing a bit shift
     *
     * \param microphone_data Pointer to the buffer containing the microphone data.
     * \param buffer_size The size of the buffer.
     */
    void remove_offset_and_scale(int16_t *microphone_data, size_t buffer_size);

private:
    uint gpio_pin; /*!< GPIO pin for ADC input */
};

#endif // MICROPHONE_H
