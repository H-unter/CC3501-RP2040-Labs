#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class Accelerometer {
public:
    Accelerometer(i2c_inst_t* i2c_instance, uint8_t sda_pin, uint8_t scl_pin, uint8_t address);

    void init();
    void get_xyz(float& x_g, float& y_g, float& z_g);

private:
    i2c_inst_t* i2c;
    uint8_t sda;
    uint8_t scl;
    uint8_t address;

    int16_t read_register_16(uint8_t reg_l, uint8_t reg_h);
    uint8_t read_register(uint8_t reg);
    void write_register(uint8_t reg, uint8_t value);
    float convert_to_g(int16_t raw_value);

    static constexpr int16_t RANGE_GS = 4;  // ±2g range
    static constexpr int BITS = 16;         // 16-bit accelerometer data
};

#endif // ACCELEROMETER_H
