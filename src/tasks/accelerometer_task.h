
#include "drivers/leds/led_array.h"
#include "drivers/leds/colour.h"
#include "drivers/accelerometer/accelerometer.h"
void set_led_based_on_accel(float g_value, int led_start_index, led_array &leds, const colour &led_colour);
int run_accelerometer_task();