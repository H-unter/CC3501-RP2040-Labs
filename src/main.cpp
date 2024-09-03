#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

#define LED_PIN 14

int main()
{
    stdio_init_all();

    // Initialise PIO0 to control the LED chain
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
    uint32_t led_data [12] = {0};


    for (;;) { // Infinite loop to continuously run the following code

        // Test the log system
        log(LogLevel::INFORMATION, "Hello world"); // Log an informational message "Hello world"

        // Turn off the first LED and turn on the second LED to be a certain color
        uint8_t red = 255;       // Set first LED red intensity 
        uint8_t green = 100;     // Set first LED green intensity 
        uint8_t blue = 255;      // Set first LED blue intensity 

        led_data[0] = (red << 24) | (green << 16) | (blue<<8); // Set the LED's data
        led_data[2] = (red << 24) | (green << 16) | (blue<<8); // Set the LED's data
        led_data[4] = (red << 24) | (green << 16) | (blue<<8); // Set the ED's data
        led_data[6] = (red << 24) | (green << 16) | (blue<<8); // Set the LED's data

        // Send data to both LEDs
        pio_sm_put_blocking(pio0, 0, led_data[0]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[1]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[2]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[3]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[4]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[5]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[6]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[7]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[8]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[9]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[10]); // Send the LED's data
        pio_sm_put_blocking(pio0, 0, led_data[11]); // Send the LED's data

        sleep_ms(500); 

         
    }




    return 0;
}
