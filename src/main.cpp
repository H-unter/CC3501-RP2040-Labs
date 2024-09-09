#include <stdio.h>
#include "pico/stdlib.h"
#include "tasks/led_task.h"  // Include the header for the task function
#include "board.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/leds/led_array.h"
#include "drivers/leds/colour.h"

#define WHO_AM_I_REG 0x0F

volatile bool stop_task = false;
// Interrupt handler for the button
void stop_task_interrupt(uint gpio, uint32_t events) {
    stop_task = true;  // Set the flag to stop the LED task
}


int main()
{
    stdio_init_all();
    //led_array leds; // Create an instance of the leds class
    //leds.init(LED_PIN, 12);
    
    // initialize the I2C hardware with 400 kHz speed
    i2c_init(ACCEL_I2C_INSTANCE, 400 * 1000);

    // Set the SDA and SCL pins for I2C
    gpio_set_function(ACCEL_SDA, GPIO_FUNC_I2C);
    gpio_set_function(ACCEL_SCL, GPIO_FUNC_I2C);

    // Prepare the register to read
    uint8_t reg = WHO_AM_I_REG;   // WHO_AM_I register address
    uint8_t data;                 // Variable to store the read data
    uint8_t who_am_i = 0;         // Variable to store the final WHO_AM_I value

    // Write the register address we want to read from
    int result = i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, &reg, 1, true);  // Send register address, keep the bus active (true)
    if (result < 0) {
        printf("Failed to write to I2C device\n");
    } else {
        // Read 1 byte from the register
        int bytes_read = i2c_read_blocking(i2c0, ACCEL_I2C_ADDRESS, &data, 1, false);  // Read the data and send stop condition (false)
        
        if (bytes_read < 0) {
            printf("Failed to read from I2C device\n");
        } else {
            who_am_i = data;
        }
    }

    // Print the result of WHO_AM_I check
    if (who_am_i == 0x33) {
        printf("WHO_AM_I register read successfully: 0x%x\n", who_am_i);
    } else {
        printf("WHO_AM_I register read failed, got: 0x%x\n", who_am_i);
    }

    return 0;
}
