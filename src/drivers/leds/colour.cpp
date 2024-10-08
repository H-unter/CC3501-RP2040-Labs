#include "colour.h"

// Constructor that initializes the color with R, G, and B values
colour::colour(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {
    update_hsv();  // Update HSV values based on the initial RGB values
}

// Getter methods
uint8_t colour::get_red() const { return red; }
uint8_t colour::get_green() const { return green; }
uint8_t colour::get_blue() const { return blue; }
uint8_t colour::get_hue() const { return hue; }
uint8_t colour::get_saturation() const { return saturation; }
uint8_t colour::get_value() const { return value; }

// Setter methods for RGB values
void colour::set_red(uint8_t r) { red = r; update_hsv(); }
void colour::set_green(uint8_t g) { green = g; update_hsv(); }
void colour::set_blue(uint8_t b) { blue = b; update_hsv(); }

// Setter methods for HSV values
void colour::set_hue(uint8_t h) {
    hue = h;
    handle_hue_overflow();  // Ensure hue stays within 0-255
    update_rgb();
}

void colour::set_saturation(uint8_t s) {
    saturation = s;
    update_rgb();
}

void colour::set_value(uint8_t v) {
    value = v;
    update_rgb();
}

// Private methods
void colour::update_hsv() {
    uint8_t cmax = std::max(red, std::max(green, blue)); // Max of R, G, B
    uint8_t cmin = std::min(red, std::min(green, blue)); // Min of R, G, B
    uint8_t diff = cmax - cmin;                          // Difference between max and min

    value = cmax;  // Calculate V (value)
    saturation = (cmax == 0) ? 0 : (diff * 255) / cmax;  // Calculate S (saturation)

    if (diff == 0) {
        hue = 0;
    } else if (cmax == red) {
        hue = 60 * ((green - blue) / diff);
    } else if (cmax == green) {
        hue = 60 * ((blue - red) / diff) + 120;
    } else if (cmax == blue) {
        hue = 60 * ((red - green) / diff) + 240;
    }

    if (hue < 0) {
        hue += 360;
    }

    hue = (hue * 255) / 360;  // Normalize hue to fit within 0-255
    handle_hue_overflow();     // Ensure hue stays within 0-255
}

void colour::update_rgb() {
    uint8_t region = (hue * 6) / 255;
    uint8_t remainder = (hue * 6) % 255;
    uint8_t p = (value * (255 - saturation)) / 255;
    uint8_t q = (value * (255 - ((saturation * remainder) / 255))) / 255;
    uint8_t t = (value * (255 - ((saturation * (255 - remainder)) / 255))) / 255;

    switch (region) {
        case 0:
            red = value;
            green = t;
            blue = p;
            break;
        case 1:
            red = q;
            green = value;
            blue = p;
            break;
        case 2:
            red = p;
            green = value;
            blue = t;
            break;
        case 3:
            red = p;
            green = q;
            blue = value;
            break;
        case 4:
            red = t;
            green = p;
            blue = value;
            break;
        default: // case 5:
            red = value;
            green = p;
            blue = q;
            break;
    }
}

void colour::handle_hue_overflow() {
    if (hue > 255) {
        hue %= 256;  // Ensure hue is within the range 0-255
    }
}
