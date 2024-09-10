#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

/*! \brief Accelerometer class for interfacing with the MMA8652FC accelerometer.
 *
 * This class provides methods to initialize the accelerometer, read the X, Y, and Z acceleration data in g's,
 * and convert the raw 16-bit accelerometer data to g's. The accelerometer is connected via I2C and uses the
 * MMA8652FC driver.
 */
class Accelerometer {
public:
    Accelerometer(i2c_inst_t* i2c_instance, uint8_t sda_pin, uint8_t scl_pin, uint8_t address);
    
    /*! \brief Initializes the I2C communication and sets up the accelerometer.
     *
     * This method initializes the I2C hardware with a 400 kHz speed, sets the SDA and SCL pins for I2C communication,
     * and verifies the accelerometer by reading the WHO_AM_I register. It also sets the CTRL_REG1 register to enable
     * the accelerometer with a 50 Hz data rate.
    */
    void init();

    /*! \brief Reads the X, Y, and Z acceleration data in g's.
     *
     * This method reads the X, Y, and Z acceleration data from the accelerometer and converts the raw 16-bit data
     * to g's. The data is stored in the provided pointers `x_g`, `y_g`, and `z_g`.
     *
     * \param x_g Pointer to the int16_t X-axis acceleration data in g's.
     * \param y_g Pointer to the int16_t Y-axis acceleration data in g's.
     * \param z_g Pointer to the int16_t Z-axis acceleration data in g's.
    */
    void get_xyz_gs(float* x_g, float* y_g, float* z_g);  // Ensure this declaration is present

private:
    i2c_inst_t* i2c;
    uint8_t sda;
    uint8_t scl;
    uint8_t address;

    int16_t read_register_16(uint8_t reg_l, uint8_t reg_h);
    uint8_t read_register(uint8_t reg);
    void write_register(uint8_t reg, uint8_t value);
    float convert_to_g(int16_t raw_value);

    int set_scale(int scale);

    /*! \brief Set the data rate of the accelerometer
    * 
    * This method sets the data rate of the accelerometer to the specified value. The data rate determines how often the accelerometer
    * samples the acceleration data. The available data rates are 1, 10, 25, 50, 100, 200, 400, 1600, 1344, and 5376 Hz.
    * 
    * \param rate The desired data rate in Hz. Must be one of the available data rates.
    * \return 0 if the data rate was set successfully, -1 if the data rate is invalid or the write operation failed.
    */
    int set_data_rate(int rate);

    static constexpr int16_t RANGE_GS = 4;  // ±2g range
    static constexpr int BITS = 16;         // 16-bit accelerometer data
};

#endif // ACCELEROMETER_H
