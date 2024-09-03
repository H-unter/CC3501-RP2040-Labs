#ifndef COLOUR_H
#define COLOUR_H

#include <cstdint>

class colour {
public:
    // Constructor that initializes the color with R, G, and B values
    colour(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

    // Getter methods for each color component
    uint8_t get_red() const { return red; }
    uint8_t get_green() const { return green; }
    uint8_t get_blue() const { return blue; }

    // Setter methods for each color component
    void set_red(uint8_t r) { red = r; }
    void set_green(uint8_t g) { green = g; }
    void set_blue(uint8_t b) { blue = b; }

private:
    uint8_t red, green, blue;
};

#endif // COLOUR_H
