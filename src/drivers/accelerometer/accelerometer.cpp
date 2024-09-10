#include "accelerometer.h"
#include <stdio.h>

// Constructor
Accelerometer::Accelerometer(i2c_inst_t* i2c_instance, uint8_t sda_pin, uint8_t scl_pin, uint8_t address)
    : i2c(i2c_instance), sda(sda_pin), scl(scl_pin), address(address) {
}

// Initialize the I2C communication and set up the accelerometer
void Accelerometer::init() {
    // Initialize the I2C hardware with 400 kHz speed
    i2c_init(i2c, 400 * 1000);

    // Set the SDA and SCL pins for I2C
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    // Verify the device by reading the WHO_AM_I register
    uint8_t who_am_i = read_register(0x0F);  // WHO_AM_I register
    if (who_am_i == 0x33) {
        printf("WHO_AM_I register read successfully: 0x%x\n", who_am_i);
    } else {
        printf("WHO_AM_I register read failed, got: 0x%x\n", who_am_i);
    }

    // Set CTRL_REG1 to enable the accelerometer with 50Hz data rate (0b01000111)
    write_register(0x20, 0b01000111);  // CTRL_REG1
}

// Get X, Y, and Z values in g's
void Accelerometer::get_xyz(float& x_g, float& y_g, float& z_g) {
    // Read the raw 16-bit X, Y, Z data from the accelerometer
    int16_t x_raw = read_register_16(0x28, 0x29);  // OUT_X_L, OUT_X_H
    int16_t y_raw = read_register_16(0x2A, 0x2B);  // OUT_Y_L, OUT_Y_H
    int16_t z_raw = read_register_16(0x2C, 0x2D);  // OUT_Z_L, OUT_Z_H

    // Convert raw data to g's
    x_g = convert_to_g(x_raw);
    y_g = convert_to_g(y_raw);
    z_g = convert_to_g(z_raw);

    printf("X: %.2f g, Y: %.2f g, Z: %.2f g\n", x_g, y_g, z_g);
}

// Convert raw 16-bit accelerometer data to g's
float Accelerometer::convert_to_g(int16_t raw_value) {
    float gs_per_bit = static_cast<float>(RANGE_GS) / (1 << BITS);  // Calculate the g's per bit
    return raw_value * gs_per_bit;  // Convert raw data to g's
}

// Helper method to read 16-bit (two registers) data
int16_t Accelerometer::read_register_16(uint8_t reg_l, uint8_t reg_h) {
    uint8_t low_byte = read_register(reg_l);
    uint8_t high_byte = read_register(reg_h);
    return (int16_t)(high_byte << 8 | low_byte);
}

// Helper method to read 8-bit data from a register
uint8_t Accelerometer::read_register(uint8_t reg) {
    uint8_t data;
    i2c_write_blocking(i2c, address, &reg, 1, true);  // Send register address
    i2c_read_blocking(i2c, address, &data, 1, false);  // Read data
    return data;
}

// Helper method to write 8-bit data to a register
void Accelerometer::write_register(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(i2c, address, buf, 2, false);  // Write register and value
}
