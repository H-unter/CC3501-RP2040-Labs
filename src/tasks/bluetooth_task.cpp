#include <stdio.h>
#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 8
#define UART_RX_PIN 9
extern volatile bool stop_task;
#include "board.h"
#include "drivers/accelerometer/accelerometer.h"

void run_bluetooth_task()
{

    uart_init(UART_ID, BAUD_RATE); // board.h
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    Accelerometer accel(ACCEL_I2C_INSTANCE, ACCEL_SDA, ACCEL_SCL, ACCEL_I2C_ADDRESS);
    accel.init(); // Initialize the accelerometer
    while (!stop_task)
    {
        float x_g, y_g, z_g;
        accel.get_xyz_gs(&x_g, &y_g, &z_g); // Read and print the accelerometer data in g's
        static char buf[100];
        snprintf(buf, 100, "%f,%f,%f\n", x_g, y_g, z_g);
        uart_puts(UART_ID, buf);

        sleep_ms(1000);
    }
}