#ifndef MICROPHONE_TASK_H
#define MICROPHONE_TASK_H
#include "hardware/adc.h"
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "drivers/microphone/microphone.h"

extern volatile bool stop_task;

int run_microphone_task();

#endif