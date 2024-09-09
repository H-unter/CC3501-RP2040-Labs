#include <stdio.h>
#include "pico/stdlib.h"
#include "tasks/led_task.h"  // Include the header for the task function
#include "board.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/leds/led_array.h"
#include "drivers/leds/colour.h"

volatile bool stop_task = false;
// Interrupt handler for the button
void stop_task_interrupt(uint gpio, uint32_t events) {
    stop_task = true;  // Set the flag to stop the LED task
}

int main()
{
    stdio_init_all();
    gpio_init(SW1);
    gpio_set_irq_enabled_with_callback(SW1, GPIO_IRQ_EDGE_RISE, true, &stop_task_interrupt);


    // Call the LED task function
    run_led_task();

    return 0;
}
