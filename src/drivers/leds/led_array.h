#ifndef LED_ARRAY_H
#define LED_ARRAY_H

#include "colour.h"  // Include the colour class
#include "hardware/pio.h"

// Declaration of the led_array class
class led_array {
public:
    // Constructor
    led_array();

    // Initializes the LED array with a specified pin and number of LEDs
    void init(uint pin, int num_leds);

    // Sets the color of an individual LED in the array
    void set_colour_individual(int index, colour colour);

    // Sets the color of a specific range of LEDs
    void set_range_color(int indices[], colour colour);

    // Sets the color of all LEDs except those in a specified range
    void set_excluded_range_color(int indices[], colour colour);

    // Clears the color of all LEDs in the array
    void clear_all();

private:
    // Converts RGB color values into a 32-bit data format
    uint32_t colour_to_led_data(colour colour);

    // Updates the LED array to reflect the current color settings
    void update_leds();

    // Member variables
    uint32_t led_data[100];  // Array to store color data for each LED
    uint led_pin;            // The pin used for controlling the LED array
    int num_leds;            // Number of LEDs in the array
};

#endif // LED_ARRAY_H
