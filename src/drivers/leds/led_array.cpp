#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"

class led_array {
public:
    
    /*! \brief Default constructor that initialises the LED array object.
     *
     * This constructor initializes the `led_pin` and `num_leds` members but does not perform any 
     * setup operations. The actual initialization of the LED array should be done using the `init()` method.
     */
    led_array()
        : led_pin(0), num_leds(0) {
    }

    /*! \brief Initialises the LED array with a specified pin and number of LEDs.
     * \ingroup pico_stdio
     *
     * This method sets up the LED array by initialising the given number of LEDs. 
     * It also configures the PIO (Programmable I/O) and the program offset for controlling the LED chain.
     *
     * \param pin The GPIO pin used to control the LED array.
     * \param num_leds The number of LEDs in the array.
     */
    void init(uint pin, int num_leds) {
        // Assign the parameters to the class members
        this->led_pin = pin;
        this->num_leds = num_leds;

        // Initialize the LED data array to 0
        for (int i = 0; i < this->num_leds; i++) {
            led_data[i] = 0;
        }

        // Initialize PIO and program offset
        uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
        ws2812_program_init(pio0, 0, pio_program_offset, led_pin, 800000, false);

        clear_all(); // Initialize all LEDs to off
    }

    /*! \brief Sets the color of an individual LED in the array.
    * \ingroup pico_stdio
    *
    * This function sets the RGB color of a specific LED in the array by providing the LED's index
    * and the red, green, and blue values. The RGB values are combined into a 32-bit data format and 
    * stored in the `led_data` array. The LED array is then updated to reflect the new color.
    *
    * \param index The index of the LED to be updated. Must be within the bounds of the LED array (0 to num_leds - 1).
    * \param red The intensity of the red color component (0 to 255).
    * \param green The intensity of the green color component (0 to 255).
    * \param blue The intensity of the blue color component (0 to 255).
    *
    * \note If the provided `index` is out of bounds, the function does nothing.
    */
    void set_individual_rgb(int index, uint8_t red, uint8_t green, uint8_t blue) {
        if (index >= 0 && index < num_leds) {
            led_data[index] = rgb_to_led_data(red, green, blue);
            update_leds();
        }
    }

    /*! \brief Sets the color of a specific range of LEDs in the array.
    * \ingroup pico_stdio
    *
    * This function sets the RGB color of the LEDs specified by the `indices` array. 
    * The RGB values are combined into a 32-bit data format and applied to each LED in the specified range.
    * The LED array is then updated to reflect the new colors.
    *
    * \param indices An array of LED indices that should be updated. The array should be terminated 
    *                with a sentinel value of `-1` to indicate the end of the range.
    * \param red The intensity of the red color component (0 to 255).
    * \param green The intensity of the green color component (0 to 255).
    * \param blue The intensity of the blue color component (0 to 255).
    *
    * \note If an index in the `indices` array is out of bounds, that index will be ignored.
    */
    void set_range_rgb(int indices[], uint8_t red, uint8_t green, uint8_t blue) {
        uint32_t color = rgb_to_led_data(red, green, blue);

        for (int i = 0; i < num_leds; i++) {
            for (int j = 0; j < num_leds; j++) {  // Check if i is in the indices array
                if (indices[j] == -1) {  // Sentinel value to end the loop
                    break;
                }
                if (indices[j] == i) {
                    led_data[i] = color;
                    break;
                }
            }
        }

        update_leds();  // Update the LEDs after setting all colors
    }

    /*! \brief Sets the color of all LEDs except those in a specified range.
    * \ingroup pico_stdio
    *
    * This function sets the RGB color of all LEDs except those specified by the `indices` array. 
    * The RGB values are combined into a 32-bit data format and applied to all LEDs not in the specified range.
    * The LED array is then updated to reflect the new colors.
    *
    * \param indices An array of LED indices that should not be updated. The array should be terminated 
    *                with a sentinel value of `-1` to indicate the end of the range.
    * \param red The intensity of the red color component (0 to 255).
    * \param green The intensity of the green color component (0 to 255).
    * \param blue The intensity of the blue color component (0 to 255).
    *
    * \note If an index in the `indices` array is out of bounds, that index will be ignored.
    */
    void set_all_but_range_rgb(int indices[], uint8_t red, uint8_t green, uint8_t blue) {
        uint32_t color = rgb_to_led_data(red, green, blue);

        for (int i = 0; i < num_leds; i++) {
            bool in_range = false;
            for (int j = 0; j < num_leds; j++) {  // Check if i is in the indices array
                if (indices[j] == -1) {  // Sentinel value to end the loop
                    break;
                }
                if (indices[j] == i) {
                    in_range = true;
                    break;
                }
            }
            if (!in_range) {
                led_data[i] = color;
            }
        }

        update_leds();  // Update the LEDs after setting all colors
    }

    /*! \brief Clears the color of all LEDs in the array.
    * \ingroup pico_stdio
    *
    * This function sets the RGB color of all LEDs in the array to black (off).
    * The LED array is then updated to reflect the new colors.
    */
    void clear_all() {
        for (int i = 0; i < num_leds; i++) {
            led_data[i] = 0;
        }
        update_leds();
    }

private:
    uint32_t led_data[100];  // Fixed-size array on the stack
    uint led_pin;
    int num_leds;  // Number of LEDs in the array

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
    void update_leds() {
        for (int i = 0; i < num_leds; i++) {
            pio_sm_put_blocking(pio0, 0, led_data[i]);
        }
        sleep_us(700);  // Adjust the sleep time based on your requirements
    }
    
    /*! \brief Converts RGB color values into a 32-bit data format for the LED array.
    * \ingroup pico_stdio
    *
    * This function takes the red, green, and blue color components as input and combines them into a single
    * 32-bit integer. The resulting value is formatted as 0xRRGGBB00, with the red component occupying the most 
    * significant byte, followed by the green and blue components.
    *
    * \param red The intensity of the red color component (0 to 255).
    * \param green The intensity of the green color component (0 to 255).
    * \param blue The intensity of the blue color component (0 to 255).
    * \return A 32-bit integer representing the combined RGB color, suitable for use in the LED data array.
    */
    uint32_t rgb_to_led_data(uint8_t red, uint8_t green, uint8_t blue) {
        return (red << 24) | (green << 16) | (blue << 8);
    }

};
