#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/leds/led_array.cpp"

#define LED_PIN 14

int main()
{
    stdio_init_all();

    led_array leds; // Create an instance of the leds class
    leds.init(LED_PIN, pio0, 0, 800000); 
    int led_range[] = {0, 1, 2, 3, 4, -1};  // Define an array of LED indices, with -1 as the sentinel value


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
        leds.set_range_rgb(led_range, 255, 0, 0); // Set the led_range LEDs to red
        sleep_ms(500);
    }




    return 0;
}
