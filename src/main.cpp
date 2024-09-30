#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include "pico/stdlib.h"
#include "tasks/led_task.h"
#include "tasks/accelerometer_task.h"

#include "board.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/leds/led_array.h"
#include "drivers/leds/colour.h"
#include "drivers/accelerometer/accelerometer.h"

volatile bool stop_task = false;

// Interrupt handler for the button
void stop_task_interrupt(uint gpio, uint32_t events)
{
    stop_task = true; // Set the flag to stop the current task
}

int increment_task_number(int number_tasks, int task_index)
{
    return (task_index + 1) % number_tasks;
}

int main()
{
    stdio_init_all();
    gpio_init(SW1);
    gpio_set_irq_enabled_with_callback(SW1, GPIO_IRQ_EDGE_FALL, true, &stop_task_interrupt); // Interrupt-based button press handling

    int number_of_tasks = 2;
    int task_index = 0;

    while (true)
    {
        if (stop_task) // Check if task should be switched
        {
            task_index = increment_task_number(number_of_tasks, task_index);
            stop_task = false; // Reset stop flag for the next task

            switch (task_index)
            {
            case 0:
                run_led_task(); // Ensure this checks stop_task and exits when true
                break;
            case 1:
                run_accelerometer_task(); // Ensure this checks stop_task and exits when true
                break;
            default:
                break;
            }
        }

        sleep_ms(50); // Optional: add a short delay to reduce CPU usage
    }

    return 0;
}
