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

// Global variables
volatile bool stop_task = false; // Flag to stop the current task
volatile int task_index = 0;     // Track the current task

// Increment task number, ensure task_index is updated in the interrupt
void increment_task_number(int number_tasks)
{
    task_index = (task_index + 1) % number_tasks;
}

// Interrupt handler for button press to switch tasks
void switch_task_interrupt(uint gpio, uint32_t events)
{
    stop_task = true;         // Set the flag to stop the current task
    increment_task_number(2); // Update the task index to the next task
}

int main()
{
    stdio_init_all();
    gpio_init(SW1);
    gpio_set_irq_enabled_with_callback(SW1, GPIO_IRQ_EDGE_FALL, true, &switch_task_interrupt);
    int number_of_tasks = 2;
    task_index = 0;
    while (true)
    {
        stop_task = false; // reset the flag
        switch (task_index)
        {
        case 0:
            run_led_task();
            break;
        case 1:
            run_accelerometer_task();
            break;
        default:
            break;
        }
    }

    return 0;
}
