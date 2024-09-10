#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>
#include <algorithm>  // Include for std::max and std::min

class colour {
public:
    // Constructor that initializes the color with R, G, and B values
    colour(uint8_t r, uint8_t g, uint8_t b);

    // Getter methods for each RGB component
    /*! \brief Returns the  `uint8_t` Red Value (0-255)*/
    uint8_t get_red() const;
    /*! \brief Returns the  `uint8_t` Green Value (0-255)*/
    uint8_t get_green() const;
    /*! \brief Returns the  `uint8_t` Blue Value (0-255)*/
    uint8_t get_blue() const;

    // Getter methods for each HSV component
    /*! \brief Returns the  `uint8_t` Hue Value (0-255)*/
    uint8_t get_hue() const;
    /*! \brief Returns the  `uint8_t` Saturation Value (0-255)*/
    uint8_t get_saturation() const;
    /*! \brief Returns the  `uint8_t` Value Value (0-255)*/
    uint8_t get_value() const;

    // Setter methods for RGB values
    /*! \brief Sets the  `uint8_t` Red Value (0-255)*/
    void set_red(uint8_t r);
    /*! \brief Sets the  `uint8_t` Green Value (0-255)*/
    void set_green(uint8_t g);
    /*! \brief Sets the  `uint8_t` Blue Value (0-255)*/
    void set_blue(uint8_t b);

    // Setter methods for HSV values
    /*! \brief Sets the  `uint8_t` Hue Value (0-255)*/
    void set_hue(uint8_t h);
    /*! \brief Sets the  `uint8_t` Saturation Value (0-255)*/
    void set_saturation(uint8_t s);
    /*! \brief Sets the  `uint8_t` Value Value (0-255)*/
    void set_value(uint8_t v);

private:
    uint8_t red, green, blue;   // RGB components
    uint8_t hue, saturation, value; // HSV components

    // Private methods for internal calculations
    void update_hsv();
    void update_rgb();
    void handle_hue_overflow();
};

#endif // COLOUR_H
