#ifndef LED_ARRAY_H
#define LED_ARRAY_H

#include "colour.h"  // Include the colour class
#include "hardware/pio.h"

/*! \brief Default constructor that initialises the LED array object.
     *
     * This constructor initializes the `led_pin` and `num_leds` members but does not perform any 
     * setup operations. The actual initialization of the LED array should be done using the `init()` method.
*/
class led_array {
public:
    // Constructor
    led_array();

    /*! \brief Initialises the LED array with a specified pin and number of LEDs.
     * \ingroup pico_stdio
     *
     * This method sets up the LED array by initialising the given number of LEDs. 
     * It also configures the PIO (Programmable I/O) and the program offset for controlling the LED chain.
     *
     * \param pin The GPIO pin used to control the LED array.
     * \param num_leds The number of LEDs in the array.
     */
    void init(uint pin, int num_leds);

    /*! \brief Sets the color of an individual LED in the array.
    * \ingroup pico_stdio
    *
    * This function sets the RGB color of a specific LED in the array by providing the LED's index
    * and the red, green, and blue values. The RGB values are combined into a 32-bit data format and 
    * stored in the `led_data` array. The LED array is then updated to reflect the new color.
    *
    * \param index The index of the LED to be updated. Must be within the bounds of the LED array (0 to num_leds - 1).
    * \param colour The colour object to set the LED to
    *
    * \note If the provided `index` is out of bounds, the function does nothing.
    */
    void set_colour_individual(int index, colour colour);

    /*! \brief Sets the color of a specific range of LEDs in the array.
    * \ingroup pico_stdio
    *
    * This function sets the RGB color of the LEDs specified by the `indices` array. 
    * The RGB values are combined into a 32-bit data format and applied to each LED in the specified range.
    * The LED array is then updated to reflect the new colors.
    *
    * \param indices An array of LED indices that should be updated. The array should be terminated 
    *                with a sentinel value of `-1` to indicate the end of the range.
    * \param colour The colour object to set the LEDs to
    *
    * \note If an index in the `indices` array is out of bounds, that index will be ignored.
    */
    void set_range_color(int indices[], colour colour);

    /*! \brief Sets the color of all LEDs except those in a specified range.
    * \ingroup pico_stdio
    *
    * This function sets the RGB color of all LEDs except those specified by the `indices` array. 
    * The RGB values are combined into a 32-bit data format and applied to all LEDs not in the specified range.
    * The LED array is then updated to reflect the new colors.
    *
    * \param indices An array of LED indices that should not be updated. The array should be terminated 
    *                with a sentinel value of `-1` to indicate the end of the range.
    * \param colour The colour object to set the rest of the LEDs to
    *
    * \note If an index in the `indices` array is out of bounds, that index will be ignored.
    */
    void set_excluded_range_color(int indices[], colour colour);

    /*! \brief Clears the color of all LEDs in the array.
    * \ingroup pico_stdio
    *
    * This function sets the RGB color of all LEDs in the array to black (off).
    * The LED array is then updated to reflect the new colors.
    */
    void clear_all();

private:
    /*! \brief Updates the LED array to reflect the current color settings.
    * \ingroup pico_stdio
    *
    * This function sends the current color data stored in the `led_data` array to the LED hardware.
    * It iterates over each LED in the array and sends the corresponding color data to the PIO (Programmable I/O)
    * using the `pio_sm_put_blocking` function. After updating the LEDs, the function pauses for a brief period
    * to ensure proper timing.
    *
    * \note The sleep time can be reduced to 530us, but lower values may cause flickering or other issues.
    */
    uint32_t colour_to_led_data(colour colour);

    /*! \brief Converts RGB color values into a 32-bit data format for the LED array.
    * \ingroup pico_stdio
    *
    * This function takes the red, green, and blue color components as input and combines them into a single
    * 32-bit integer. The resulting value is formatted as 0xRRGGBB00, with the red component occupying the most 
    * significant byte, followed by the green and blue components.
    *
    * \param colour The colour object to convert to a 32-bit integer.
    * \return A 32-bit integer representing the combined RGB color, suitable for use in the LED data array.
    */
    void update_leds();

    // Member variables
    uint32_t led_data[100];  // Array to store color data for each LED
    uint led_pin;            // The pin used for controlling the LED array
    int num_leds;            // Number of LEDs in the array
};

#endif // LED_ARRAY_H
