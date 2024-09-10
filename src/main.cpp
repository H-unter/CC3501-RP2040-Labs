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

    /* 
    ------------------ INITIALIZE THE I2C HARDWARE
    */
    
    // initialize the I2C hardware with 400 kHz speed
    i2c_init(ACCEL_I2C_INSTANCE, 400 * 1000);

    // Set the SDA and SCL pins for I2C
    gpio_set_function(ACCEL_SDA, GPIO_FUNC_I2C);
    gpio_set_function(ACCEL_SCL, GPIO_FUNC_I2C);


    /* 
    ------------------ READ THE WHO_AM_I REGISTER
    */
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

    /* 
    ------------------ WRITE TO THE CTRL_REG1 REGISTER
    */
    // Prepare the register to read
    uint8_t CTRL_REG1_REG = 0x20;   // CTRL_REG1_REG register address
    uint8_t new_data = 0b01000111;  // New data to write to the register, first 4 bits are the data rate, the others are default values
    uint8_t buf[2];
    buf[0] = CTRL_REG1_REG;
    buf[1] = new_data; //need to package address and data into one "stream" to write to the register
    int result_datarate = i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, buf, 2, true);  // Send register address, keep the bus active (true)
    if (result_datarate !=2) {
        printf("Failed to write to I2C device\n");
    } else {
        printf("CTRL_REG1 register written successfully\n");
    }

    /* 
    ------------------ READ FROM THE X,Y,Z REGISTERS
    OUT_X_L (0x28 = 0101000): Lower byte of the X-axis acceleration data
    OUT_X_H (0x29): Higher byte of the X-axis acceleration data
    OUT_Y_L (0x2A): Lower byte of the Y-axis acceleration data
    OUT_Y_H (0x2B): Higher byte of the Y-axis acceleration data
    OUT_Z_L (0x2C): Lower byte of the Z-axis acceleration data
    OUT_Z_H (0x2D): Higher byte of the Z-axis acceleration data


    to enable multi byte read we change the initial address MSB to 1
    */
    // Prepare the register to read
    uint8_t OUT_X_L = 0x28;   // OUT_X_L register address = 0101000
    uint8_t OUT_X_H = 0x29;   // OUT_X_H register address
    uint8_t OUT_Y_L = 0x2A;   // OUT_Y_L register address
    uint8_t OUT_Y_H = 0x2B;   // OUT_Y_H register address
    uint8_t OUT_Z_L = 0x2C;   // OUT_Z_L register address
    uint8_t OUT_Z_H = 0x2D;   // OUT_Z_H register address
    uint8_t data_x_l, data_x_h, data_y_l, data_y_h, data_z_l, data_z_h;  // Variables to store the read data
    int16_t x_accel, y_accel, z_accel;  // Variables to store the final acceleration values

    uint8_t starting_address = 0x28 | 0x80; // set MSB to 1 to enable multi byte read

    while (true){
    // Read the X-axis acceleration data
    i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, &OUT_X_L, 1, true);  // Send register address, keep the bus active (true)
    i2c_read_blocking(i2c0, ACCEL_I2C_ADDRESS, &data_x_l, 1, false);  // Read the data and send stop condition (false)
    i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, &OUT_X_H, 1, true);  // Send register address, keep the bus active (true)
    i2c_read_blocking(i2c0, ACCEL_I2C_ADDRESS, &data_x_h, 1, false);  // Read the data and send stop condition (false)

    // Read the Y-axis acceleration data
    i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, &OUT_Y_L, 1, true);  // Send register address, keep the bus active (true)
    i2c_read_blocking(i2c0, ACCEL_I2C_ADDRESS, &data_y_l, 1, false);  // Read the data and send stop condition (false)
    i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, &OUT_Y_H, 1, true);  // Send register address, keep the bus active (true)
    i2c_read_blocking(i2c0, ACCEL_I2C_ADDRESS, &data_y_h, 1, false);  // Read the data and send stop condition (false)

    // Read the Z-axis acceleration data
    i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, &OUT_Z_L, 1, true);  // Send register address, keep the bus active (true)
    i2c_read_blocking(i2c0, ACCEL_I2C_ADDRESS, &data_z_l, 1, false);  // Read the data and send stop condition (false)
    i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, &OUT_Z_H, 1, true);  // Send register address, keep the bus active (true)
    i2c_read_blocking(i2c0, ACCEL_I2C_ADDRESS, &data_z_h, 1, false);  // Read the data and send stop condition (false)
    
    // Combine the lower and higher bytes of the X, Y, and Z acceleration data using bitwise OR

    int16_t X = (int16_t)(data_x_l | (data_x_h << 8)) >> 6;
    int16_t Y = (int16_t)(data_y_l | (data_y_h << 8)) >> 6;
    int16_t Z = (int16_t)(data_z_l | (data_z_h << 8)) >> 6;

    printf("X-axis acceleration: %d\n", X);
    printf("Y-axis acceleration: %d\n", Y);
    printf("Z-axis acceleration: %d\n", Z);
    sleep_ms(500);

    }



    return 0;
}
