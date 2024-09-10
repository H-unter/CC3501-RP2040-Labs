#include "led_array.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"

// Constructor
led_array::led_array()
    : led_pin(0), num_leds(0) {
}

// Initializes the LED array with a specified pin and number of LEDs
void led_array::init(uint pin, int num_leds) {
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

// Sets the color of an individual LED in the array
void led_array::set_colour_individual(int index, colour colour) {
    bool is_valid_index = (index >= 0 && index < num_leds);
    if (is_valid_index) {
        led_data[index] = colour_to_led_data(colour);
        update_leds();
    }
}

// Sets the color of a specific range of LEDs
void led_array::set_range_color(int indices[], colour colour) {
    uint32_t color = colour_to_led_data(colour);

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

// Sets the color of all LEDs except those in a specified range
void led_array::set_excluded_range_color(int indices[], colour colour) {
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
            led_data[i] = colour_to_led_data(colour);
        }
    }

    update_leds();  // Update the LEDs after setting all colors
}

// Clears the color of all LEDs in the array
void led_array::clear_all() {
    for (int i = 0; i < num_leds; i++) {
        led_data[i] = 0;
    }
    update_leds();
}

// Converts RGB color values into a 32-bit data format
uint32_t led_array::colour_to_led_data(colour colour) {
    return (colour.get_red() << 24) | (colour.get_green() << 16) | (colour.get_blue() << 8);
}

// Updates the LED array to reflect the current color settings
void led_array::update_leds() {
    for (int i = 0; i < num_leds; i++) {
        pio_sm_put_blocking(pio0, 0, led_data[i]);
    }
    sleep_us(700);  // Adjust the sleep time based on your requirements
}
