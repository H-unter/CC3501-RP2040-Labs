#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "board.h"
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"

#include "drivers/leds/led_array.h"
#include "drivers/leds/colour.h"

#include "led_task.h"

int run_led_task()
{
    led_array leds; // Create an instance of the leds class
    leds.init(LED_PIN, 12);
    int led_range[] = {0, 1, 2, 3, 4, -1}; // Define an array of LED indices, with -1 as the sentinel value
    colour snake_colour(255, 0, 255);      // Create a purple colour object
    colour black(0, 0, 0);                 // Create a black colour object

    while (!stop_task)
    { // Infinite loop to continuously run the following code
        for (int i = 0; i < 5; i++)
        {
            if (led_range[i] == 12)
            {
                led_range[i] = 0;
            }
            else
            {
                led_range[i]++;
            }
        }
        snake_colour.set_hue(snake_colour.get_hue() + 5); // Increment the hue value of the snake colour
        leds.set_range_color(led_range, snake_colour);
        leds.set_excluded_range_color(led_range, black);

        sleep_ms(50);
    }
    return 0;
}
