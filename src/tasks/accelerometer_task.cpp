#include <stdio.h>
#include <stdint.h> // For using uint8_t
#include <cmath>    // For exp() function
#include "pico/stdlib.h"
#include "tasks/led_task.h" // Include the header for the task function
#include "board.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/leds/led_array.h"
#include "drivers/leds/colour.h"
#include "drivers/accelerometer/accelerometer.h"

void set_led_based_on_accel(float g_value, int led_start_index, led_array &leds, const colour &led_colour)
{
    // https://www.desmos.com/calculator/ej79tccscr

    // Set the value of A for the exponential function
    float A = 3.0f;
    uint8_t x_0 = static_cast<uint8_t>(255 * exp(-A * ((-1.0f - g_value) * (-1.0f - g_value))));               // x = -1
    uint8_t x_1 = static_cast<uint8_t>(255 * exp(-A * ((-1.0f / 3.0f - g_value) * (-1.0f / 3.0f - g_value)))); // x = -1/3
    uint8_t x_2 = static_cast<uint8_t>(255 * exp(-A * ((1.0f / 3.0f - g_value) * (1.0f / 3.0f - g_value))));   // x = 1/3
    uint8_t x_3 = static_cast<uint8_t>(255 * exp(-A * ((1.0f - g_value) * (1.0f - g_value))));                 // x = 1

    // Create temporary copies of led_colour for each LED
    colour led0 = led_colour;
    colour led1 = led_colour;
    colour led2 = led_colour;
    colour led3 = led_colour;

    // Set different values for each copy
    led0.set_value(x_0);
    led1.set_value(x_1);
    led2.set_value(x_2);
    led3.set_value(x_3);

    // Set LED colours with the clamped values
    leds.set_colour_individual(led_start_index, led0);
    leds.set_colour_individual(led_start_index + 1, led1);
    leds.set_colour_individual(led_start_index + 2, led2);
    leds.set_colour_individual(led_start_index + 3, led3);
}

int run_accelerometer_task()
{
    Accelerometer accel(ACCEL_I2C_INSTANCE, ACCEL_SDA, ACCEL_SCL, ACCEL_I2C_ADDRESS);
    accel.init();   // Initialize the accelerometer
    led_array leds; // Create an instance of the leds class
    leds.init(LED_PIN, 12);
    int x_led_start_index = 4;
    int y_led_start_index = 0;
    int z_led_start_index = 8;
    colour x_base_colour(255, 0, 0); // Create a red colour object
    colour y_base_colour(0, 255, 0); // Create a green colour object
    colour z_base_colour(0, 0, 255); // Create a blue colour object

    while (!stop_task)
    {
        float x_g, y_g, z_g;

        accel.get_xyz_gs(&x_g, &y_g, &z_g); // Read and print the accelerometer data in g's
        printf("X: %.2f g, Y: %.2f g, Z: %.2f g\n", x_g, y_g, z_g);
        // divide each value by 4 to get 4 bins, (very negative, negative, positive, very positive)
        //  use this to decide which led to illuminate
        leds.clear_all();
        // Call the function for each axis
        set_led_based_on_accel(x_g, x_led_start_index, leds, x_base_colour);
        set_led_based_on_accel(y_g, y_led_start_index, leds, y_base_colour);
        set_led_based_on_accel(z_g, z_led_start_index, leds, z_base_colour);

        sleep_us(1); // Adjust the delay as needed
    }

    return 0;
}