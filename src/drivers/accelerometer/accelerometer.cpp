#include "accelerometer.h"
#include <stdio.h>
#include "board.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

// Constructor
Accelerometer::Accelerometer(i2c_inst_t *i2c_instance, uint8_t sda_pin, uint8_t scl_pin, uint8_t address)
    : i2c(i2c_instance), sda(sda_pin), scl(scl_pin), address(address)
{
}

// Initialize the I2C communication and set up the accelerometer
void Accelerometer::init()
{
    // Initialize the I2C hardware with 400 kHz speed
    i2c_init(i2c, 400 * 1000);

    // Set the SDA and SCL pins for I2C
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    // Verify the device by reading the WHO_AM_I register
    uint8_t who_am_i = read_register(0x0F); // WHO_AM_I register
    if (who_am_i == 0x33)
    {
        // printf("WHO_AM_I register read successfully: 0x%x\n", who_am_i);
    }
    else
    {
        printf("WHO_AM_I register read failed, got: 0x%x\n", who_am_i);
    }

    // Set CTRL_REG1 to enable the accelerometer with 50Hz data rate (0b01000111)
    write_register(0x20, 0b01000111); // CTRL_REG1
}

// Get X, Y, and Z values in g's
void Accelerometer::get_xyz_gs(float *x_g, float *y_g, float *z_g)
{
    // Read the X-axis acceleration data
    uint8_t xyz_starting_address = 0x28 | 0x80;                                 // Set MSB to 1 to enable multi-byte read
    uint8_t accel_read_data[6];                                                 // Array to store the read data
    i2c_write_blocking(i2c, ACCEL_I2C_ADDRESS, &xyz_starting_address, 1, true); // Send register address, keep the bus active (true)
    i2c_read_blocking(i2c, ACCEL_I2C_ADDRESS, accel_read_data, 6, false);       // Read the data and send stop condition (false)

    // Combine the high and low bytes for X, Y, Z acceleration data
    int16_t x_raw = (int16_t)(accel_read_data[1] << 8 | accel_read_data[0]); // X-axis data (High byte shifted left, OR'd with low byte)
    int16_t y_raw = (int16_t)(accel_read_data[3] << 8 | accel_read_data[2]); // Y-axis data
    int16_t z_raw = (int16_t)(accel_read_data[5] << 8 | accel_read_data[4]); // Z-axis data

    // Convert raw data to g's
    *x_g = convert_to_g(x_raw);
    *y_g = convert_to_g(y_raw);
    *z_g = convert_to_g(z_raw);
}

// Convert raw 16-bit accelerometer data to g's
float Accelerometer::convert_to_g(int16_t raw_value)
{
    float gs_per_bit = static_cast<float>(RANGE_GS) / (1 << BITS); // Calculate the g's per bit
    return raw_value * gs_per_bit;                                 // Convert raw data to g's
}

// Helper method to read 16-bit (two registers) data
int16_t Accelerometer::read_register_16(uint8_t reg_l, uint8_t reg_h)
{
    uint8_t low_byte = read_register(reg_l);
    uint8_t high_byte = read_register(reg_h);
    return (int16_t)(high_byte << 8 | low_byte);
}

// Helper method to read 8-bit data from a register
uint8_t Accelerometer::read_register(uint8_t reg)
{
    uint8_t data;
    i2c_write_blocking(i2c, address, &reg, 1, true);  // Send register address
    i2c_read_blocking(i2c, address, &data, 1, false); // Read data
    return data;
}

// Helper method to write 8-bit data to a register
void Accelerometer::write_register(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(i2c, address, buf, 2, false); // Write register and value
}

int Accelerometer::set_scale(int scale)
{
    uint8_t CTRL_REG4_REG = 0x23;                // CTRL_REG4 register address
    uint8_t current_register_value = 0b00000000; // Default value with other bits set to 0

    // Set the FS[1:0] bits for full-scale selection
    uint8_t scale_bits;

    // Switch case to handle different scale levels based on the table
    switch (scale)
    {
    case 2:
        scale_bits = 0b00 << 2; // ±2g (00) (default)
        break;
    case 4:
        scale_bits = 0b01 << 2; // ±4g (01)
        break;
    case 8:
        scale_bits = 0b10 << 2; // ±8g (10)
        break;
    case 16:
        scale_bits = 0b11 << 2; // ±16g (11)
        break;
    default:
        printf("Invalid scale: %d g. No changes made.\n", scale);
        return -1; // Error code for invalid scale
    }

    // Combine scale bits with the current register value (leave other bits unchanged)
    uint8_t new_scale_register = (scale_bits & 0x0C) | (current_register_value & 0xF3); // Modify bits 2 and 3 (0x0C is the mask)

    // Write to the CTRL_REG4 register
    uint8_t buf[2];
    buf[0] = CTRL_REG4_REG;      // Register address
    buf[1] = new_scale_register; // Data to write

    int result_scale = i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, buf, 2, true); // Send register address and data

    if (result_scale != 2)
    {
        printf("Failed to write to I2C device\n");
        return -1; // Error code for failed write
    }
    else
    {
        printf("CTRL_REG4 register written successfully. Scale set to ±%d g\n", scale);
        return 0; // Success code
    }
}

int Accelerometer::set_data_rate(int rate)
{
    uint8_t CTRL_REG1_REG = 0x20;                // CTRL_REG1 register address
    uint8_t current_register_value = 0b01100111; // Default value (except for data rate bits)

    // Set the 4 most significant bits for data rate
    uint8_t data_rate_bits;

    // Switch case to handle different data rates based on the table
    switch (rate)
    {
    case 1:
        data_rate_bits = 0b0001 << 4; // 1 Hz (0001)
        break;
    case 10:
        data_rate_bits = 0b0010 << 4; // 10 Hz (0010)
        break;
    case 25:
        data_rate_bits = 0b0011 << 4; // 25 Hz (0011)
        break;
    case 50:
        data_rate_bits = 0b0100 << 4; // 50 Hz (0100)
        break;
    case 100:
        data_rate_bits = 0b0101 << 4; // 100 Hz (0101)
        break;
    case 200:
        data_rate_bits = 0b0110 << 4; // 200 Hz (0110)
        break;
    case 400:
        data_rate_bits = 0b0111 << 4; // 400 Hz (0111)
        break;
    case 1600:
        data_rate_bits = 0b1000 << 4; // 1.60 kHz (1000)
        break;
    case 1344:
        data_rate_bits = 0b1001 << 4; // 1.344 kHz (1001)
        break;
    case 5376:
        data_rate_bits = 0b1001 << 4; // 5.376 kHz (for low-power mode) (1001)
        break;
    default:
        printf("Invalid data rate: %d Hz. No changes made.\n", rate);
        return -1; // Error code for invalid data rate
    }

    // Combine data rate bits with the default register value (lower 4 bits unchanged)
    uint8_t new_data_rate_register = (data_rate_bits & 0xF0) | (current_register_value & 0x0F);

    // Write to the CTRL_REG1 register
    uint8_t buf[2];
    buf[0] = CTRL_REG1_REG;          // Register address
    buf[1] = new_data_rate_register; // Data to write

    int result_datarate = i2c_write_blocking(i2c0, ACCEL_I2C_ADDRESS, buf, 2, true); // Send register address and data

    if (result_datarate != 2)
    {
        printf("Failed to write to I2C device\n");
        return -1; // Error code for failed write
    }
    else
    {
        printf("CTRL_REG1 register written successfully. Data rate set to %d Hz\n", rate);
        return 0; // Success code
    }
}
