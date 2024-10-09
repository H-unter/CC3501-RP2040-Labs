#ifndef MICROPHONE_TASK_H
#define MICROPHONE_TASK_H

#include "hardware/adc.h"
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "drivers/microphone/microphone.h"
#include "drivers/leds/led_array.h"
#include "drivers/leds/colour.h"
#include "arm_math.h"

extern volatile bool stop_task;

// Function declarations
void run_microphone_task();

void apply_hanning_window(int16_t time_domain_signal[], const int16_t hanning_window[], size_t sample_size);
void calculate_spectral_density(int16_t freq_domain_signal[], uint64_t spectral_density[], size_t sample_size);
void calculate_frequency_bin_sums(const uint64_t spectral_density[], uint16_t (&frequency_bin_sums)[12], uint16_t &max_bin_sum, const size_t freq_bin_boundaries[13]);
void scale_frequency_bins(const uint16_t (&frequency_bin_sums)[12], uint8_t (&scaled_frequency_bin_sums)[12], uint16_t max_bin_sum);
void update_leds(led_array &leds, const colour &base_colour, const uint8_t (&scaled_frequency_bin_sums)[12]);

#endif
