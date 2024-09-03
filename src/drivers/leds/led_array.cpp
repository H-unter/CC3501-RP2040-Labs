#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"

class led_array {
public:
    led_array(){
        // empty constructor since using the init function seems nicer
        printf("leds constructor\n");
    }

    void init(uint pin, PIO pio_instance, uint sm_instance, uint freq) {
        // Initialization code
        led_pin = pin;
        pio = pio_instance;
        sm = sm_instance; // Initialize the state machine
        frequency = freq;
        // Initialize the class member led_data array
        for (int i = 0; i < 12; i++) {
            led_data[i] = 0;
        }

        // Initialise PIO to control the LED chain
        pio_program_offset = pio_add_program(pio, &ws2812_program);
        ws2812_program_init(pio, sm, pio_program_offset, led_pin, frequency, false);
        clear_all(); // Clear all LEDs
    }

    void set_individual_rgb(int index, uint8_t red, uint8_t green, uint8_t blue) {
        bool is_valid_index = index >= 0 && index < 12;
        if (is_valid_index) {
            led_data[index] = rgb_to_led_data(red, green, blue);
            update_leds();
        }
        
    }

    void set_range_rgb(int indices[], uint8_t red, uint8_t green, uint8_t blue) {
        for (size_t i = 0; i < 12; i++) {
            for (size_t j = 0; j < 12; j++) {  // Check if i is in the indices array
                if (indices[j] == -1) {  // Sentinel value to end the loop
                    break;
                }
                if (indices[j] == i) {
                    led_data[i] = rgb_to_led_data(red, green, blue);
                    break;
                }
            }
        }

        update_leds();  // Update the LEDs after setting all colors
    }

    void set_all_but_range_rgb(int indices[], uint8_t red, uint8_t green, uint8_t blue) {
        for (size_t i = 0; i < 12; i++) {
            for (size_t j = 0; j < 12; j++) {  // Check if i is in the indices array
                if (indices[j] == -1) {  // Sentinel value to end the loop
                    break;
                }
                if (indices[j] != i) {
                    led_data[i] = rgb_to_led_data(red, green, blue);
                    break;
                }
            }
        }

        update_leds();  // Update the LEDs after setting all colors
    }


    void clear_all() {
        for (int i = 0; i < 12; i++) {
            led_data[i] = 0;
        }
        update_leds();
    }


private:
    uint32_t led_data[12];
    uint pio_program_offset;
    uint led_pin;
    PIO pio;
    uint sm;
    uint frequency;


    void update_leds() {
        for (int i = 0; i < 12; i++) {
            pio_sm_put_blocking(pio, sm, led_data[i]);
        }
        //sleep_us(530); // this is the smallest time that could be used whilst not having timing issues
        sleep_us(700);
    }

    uint32_t rgb_to_led_data(uint8_t red, uint8_t green, uint8_t blue) {
        return (red << 24) | (green << 16) | (blue<<8);
    }
};