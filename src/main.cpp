#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/leds/led_array.cpp"
#include "drivers/leds/colour.h"

#define LED_PIN 14

int main()
{
    stdio_init_all();

    led_array leds; // Create an instance of the leds class
    leds.init(LED_PIN, 12); 
    int led_range[] = {0, 1, 2, 3, 4, -1};  // Define an array of LED indices, with -1 as the sentinel value

    colour snake_colour(255, 0, 255); // Create a purple colour object
    colour black(0, 0, 0); // Create a black colour object
    

    for (;;) { // Infinite loop to continuously run the following code
        for (int i=0; i<5; i++) {
            if (led_range[i] == 12) {
                led_range[i] = 0;
            }
            else {
                led_range[i]++;
            }
        }
        // Test the log system
        log(LogLevel::INFORMATION, "Hello world"); // Log an informational message "Hello world"
        snake_colour.set_hue(snake_colour.get_hue() + 5); // Increment the hue value of the snake colour
        leds.set_range_rgb(led_range, snake_colour); 
        leds.set_all_but_range_rgb(led_range, black); 

        sleep_ms(50);
    }




    return 0;
}
