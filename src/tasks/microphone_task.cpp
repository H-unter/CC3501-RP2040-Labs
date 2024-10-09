#include "microphone_task.h"
#include <stdio.h>
#include "arm_math.h"
#include "pico/stdlib.h"
#include "drivers/leds/led_array.h"
#include "drivers/leds/colour.h"
#include "board.h"
#include <stdint.h>
#include <cmath>

// Define the buffer size for the microphone and FFT
#define SAMPLE_SIZE 1024

int16_t microphone_sample_buffer[SAMPLE_SIZE];    // Buffer to store microphone samples (ADC values)
int16_t time_domain_signal[SAMPLE_SIZE];          // Buffer to store Q15 samples for FFT
int16_t freq_domain_signal[SAMPLE_SIZE + 2];      // Buffer to store FFT output (complex values)
uint64_t spectral_density[(SAMPLE_SIZE / 2) + 1]; // Buffer to store magnitude squared results

// Hanning window coefficients (MATLAB generated values)
int16_t hanning_window[SAMPLE_SIZE] = {0, 0, 1, 3, 5, 8, 11, 15, 20, 25, 31, 37, 44, 52, 61, 69, 79, 89, 100, 111, 123, 136, 149, 163, 178, 193, 208, 225, 242, 259, 277, 296, 315, 335, 356, 377, 399, 421, 444, 468, 492, 517, 542, 568, 595, 622, 650, 678, 707, 736, 767, 797, 829, 860, 893, 926, 960, 994, 1029, 1064, 1100, 1137, 1174, 1211, 1250, 1288, 1328, 1368, 1408, 1449, 1491, 1533, 1576, 1619, 1663, 1708, 1753, 1798, 1844, 1891, 1938, 1986, 2034, 2083, 2133, 2182, 2233, 2284, 2335, 2387, 2440, 2493, 2547, 2601, 2656, 2711, 2766, 2823, 2879, 2937, 2994, 3053, 3111, 3171, 3230, 3291, 3351, 3413, 3474, 3536, 3599, 3662, 3726, 3790, 3855, 3920, 3985, 4051, 4118, 4185, 4252, 4320, 4388, 4457, 4526, 4596, 4666, 4737, 4808, 4879, 4951, 5023, 5096, 5169, 5243, 5317, 5391, 5466, 5541, 5617, 5693, 5769, 5846, 5923, 6001, 6079, 6158, 6236, 6316, 6395, 6475, 6555, 6636, 6717, 6799, 6880, 6962, 7045, 7128, 7211, 7295, 7379, 7463, 7547, 7632, 7717, 7803, 7889, 7975, 8062, 8148, 8236, 8323, 8411, 8499, 8587, 8676, 8765, 8854, 8944, 9033, 9123, 9214, 9304, 9395, 9486, 9578, 9670, 9761, 9854, 9946, 10039, 10132, 10225, 10318, 10412, 10505, 10599, 10694, 10788, 10883, 10978, 11073, 11168, 11264, 11359, 11455, 11551, 11648, 11744, 11841, 11937, 12034, 12131, 12229, 12326, 12424, 12521, 12619, 12717, 12815, 12914, 13012, 13111, 13209, 13308, 13407, 13506, 13605, 13704, 13804, 13903, 14003, 14102, 14202, 14302, 14401, 14501, 14601, 14701, 14802, 14902, 15002, 15102, 15203, 15303, 15403, 15504, 15604, 15705, 15806, 15906, 16007, 16107, 16208, 16309, 16409, 16510, 16610, 16711, 16812, 16912, 17013, 17113, 17214, 17314, 17415, 17515, 17616, 17716, 17816, 17916, 18017, 18117, 18217, 18317, 18416, 18516, 18616, 18716, 18815, 18915, 19014, 19113, 19213, 19312, 19411, 19509, 19608, 19707, 19805, 19904, 20002, 20100, 20198, 20296, 20393, 20491, 20588, 20685, 20782, 20879, 20976, 21072, 21169, 21265, 21361, 21457, 21552, 21647, 21743, 21838, 21932, 22027, 22121, 22216, 22309, 22403, 22497, 22590, 22683, 22776, 22868, 22961, 23053, 23144, 23236, 23327, 23418, 23509, 23599, 23690, 23780, 23869, 23959, 24048, 24136, 24225, 24313, 24401, 24489, 24576, 24663, 24750, 24836, 24922, 25008, 25093, 25178, 25263, 25347, 25431, 25515, 25599, 25682, 25764, 25847, 25929, 26010, 26091, 26172, 26253, 26333, 26413, 26492, 26571, 26650, 26728, 26806, 26883, 26960, 27037, 27113, 27189, 27265, 27340, 27414, 27488, 27562, 27636, 27708, 27781, 27853, 27925, 27996, 28067, 28137, 28207, 28276, 28345, 28414, 28482, 28550, 28617, 28683, 28750, 28815, 28881, 28946, 29010, 29074, 29137, 29200, 29263, 29325, 29386, 29447, 29508, 29568, 29627, 29686, 29745, 29803, 29860, 29917, 29974, 30029, 30085, 30140, 30194, 30248, 30301, 30354, 30407, 30458, 30510, 30560, 30611, 30660, 30709, 30758, 30806, 30853, 30900, 30947, 30993, 31038, 31083, 31127, 31170, 31213, 31256, 31298, 31339, 31380, 31420, 31460, 31499, 31538, 31576, 31613, 31650, 31686, 31722, 31757, 31791, 31825, 31859, 31891, 31924, 31955, 31986, 32017, 32046, 32076, 32104, 32132, 32160, 32187, 32213, 32239, 32264, 32288, 32312, 32335, 32358, 32380, 32402, 32422, 32443, 32462, 32481, 32500, 32518, 32535, 32551, 32567, 32583, 32598, 32612, 32625, 32638, 32651, 32662, 32673, 32684, 32694, 32703, 32712, 32720, 32727, 32734, 32740, 32746, 32751, 32755, 32759, 32762, 32764, 32766, 32767, 32767, 32767, 32767, 32766, 32764, 32762, 32759, 32755, 32751, 32746, 32740, 32734, 32727, 32720, 32712, 32703, 32694, 32684, 32673, 32662, 32651, 32638, 32625, 32612, 32598, 32583, 32567, 32551, 32535, 32518, 32500, 32481, 32462, 32443, 32422, 32402, 32380, 32358, 32335, 32312, 32288, 32264, 32239, 32213, 32187, 32160, 32132, 32104, 32076, 32046, 32017, 31986, 31955, 31924, 31891, 31859, 31825, 31791, 31757, 31722, 31686, 31650, 31613, 31576, 31538, 31499, 31460, 31420, 31380, 31339, 31298, 31256, 31213, 31170, 31127, 31083, 31038, 30993, 30947, 30900, 30853, 30806, 30758, 30709, 30660, 30611, 30560, 30510, 30458, 30407, 30354, 30301, 30248, 30194, 30140, 30085, 30029, 29974, 29917, 29860, 29803, 29745, 29686, 29627, 29568, 29508, 29447, 29386, 29325, 29263, 29200, 29137, 29074, 29010, 28946, 28881, 28815, 28750, 28683, 28617, 28550, 28482, 28414, 28345, 28276, 28207, 28137, 28067, 27996, 27925, 27853, 27781, 27708, 27636, 27562, 27488, 27414, 27340, 27265, 27189, 27113, 27037, 26960, 26883, 26806, 26728, 26650, 26571, 26492, 26413, 26333, 26253, 26172, 26091, 26010, 25929, 25847, 25764, 25682, 25599, 25515, 25431, 25347, 25263, 25178, 25093, 25008, 24922, 24836, 24750, 24663, 24576, 24489, 24401, 24313, 24225, 24136, 24048, 23959, 23869, 23780, 23690, 23599, 23509, 23418, 23327, 23236, 23144, 23053, 22961, 22868, 22776, 22683, 22590, 22497, 22403, 22309, 22216, 22121, 22027, 21932, 21838, 21743, 21647, 21552, 21457, 21361, 21265, 21169, 21072, 20976, 20879, 20782, 20685, 20588, 20491, 20393, 20296, 20198, 20100, 20002, 19904, 19805, 19707, 19608, 19509, 19411, 19312, 19213, 19113, 19014, 18915, 18815, 18716, 18616, 18516, 18416, 18317, 18217, 18117, 18017, 17916, 17816, 17716, 17616, 17515, 17415, 17314, 17214, 17113, 17013, 16912, 16812, 16711, 16610, 16510, 16409, 16309, 16208, 16107, 16007, 15906, 15806, 15705, 15604, 15504, 15403, 15303, 15203, 15102, 15002, 14902, 14802, 14701, 14601, 14501, 14401, 14302, 14202, 14102, 14003, 13903, 13804, 13704, 13605, 13506, 13407, 13308, 13209, 13111, 13012, 12914, 12815, 12717, 12619, 12521, 12424, 12326, 12229, 12131, 12034, 11937, 11841, 11744, 11648, 11551, 11455, 11359, 11264, 11168, 11073, 10978, 10883, 10788, 10694, 10599, 10505, 10412, 10318, 10225, 10132, 10039, 9946, 9854, 9761, 9670, 9578, 9486, 9395, 9304, 9214, 9123, 9033, 8944, 8854, 8765, 8676, 8587, 8499, 8411, 8323, 8236, 8148, 8062, 7975, 7889, 7803, 7717, 7632, 7547, 7463, 7379, 7295, 7211, 7128, 7045, 6962, 6880, 6799, 6717, 6636, 6555, 6475, 6395, 6316, 6236, 6158, 6079, 6001, 5923, 5846, 5769, 5693, 5617, 5541, 5466, 5391, 5317, 5243, 5169, 5096, 5023, 4951, 4879, 4808, 4737, 4666, 4596, 4526, 4457, 4388, 4320, 4252, 4185, 4118, 4051, 3985, 3920, 3855, 3790, 3726, 3662, 3599, 3536, 3474, 3413, 3351, 3291, 3230, 3171, 3111, 3053, 2994, 2937, 2879, 2823, 2766, 2711, 2656, 2601, 2547, 2493, 2440, 2387, 2335, 2284, 2233, 2182, 2133, 2083, 2034, 1986, 1938, 1891, 1844, 1798, 1753, 1708, 1663, 1619, 1576, 1533, 1491, 1449, 1408, 1368, 1328, 1288, 1250, 1211, 1174, 1137, 1100, 1064, 1029, 994, 960, 926, 893, 860, 829, 797, 767, 736, 707, 678, 650, 622, 595, 568, 542, 517, 492, 468, 444, 421, 399, 377, 356, 335, 315, 296, 277, 259, 242, 225, 208, 193, 178, 163, 149, 136, 123, 111, 100, 89, 79, 69, 61, 52, 44, 37, 31, 25, 20, 15, 11, 8, 5, 3, 1, 0, 0};
//                                  1  2   3   4   5   6   7   8    9   10   11    12
/*! \brief Function to run the microphone task, reading samples from the microphone and performing FFT.
 *
 * This function initializes the microphone, reads audio samples, converts them to Q15 format,
 * performs FFT, and computes the magnitude squared of the FFT result.
 */
void run_microphone_task()
{
    led_array leds;                  // Create an instance of the LED array class
    leds.init(LED_PIN, 12);          // Initialize LED array with 12 LEDs
    leds.clear_all();                // Clear all LEDs
    colour base_colour(0, 255, 255); // Base colour for LEDs (cyan)

    // Create an instance of the microphone class
    microphone mic;

    // Initialize the microphone
    mic.init(26);

    // FFT instance and parameters
    uint32_t is_inverse_fft = 0;                                                     // 0 for forward FFT
    uint32_t is_bits_reversed = 1;                                                   // Enable bit-reversal of output
    arm_rfft_instance_q15 fft_instance;                                              // Create the FFT instance
    arm_rfft_init_q15(&fft_instance, SAMPLE_SIZE, is_inverse_fft, is_bits_reversed); // Initialize FFT for 1024-point FFT

    // Main task loop
    while (!stop_task)
    {
        // Read microphone data
        mic.read_blocking(time_domain_signal, SAMPLE_SIZE); // Blocking read until buffer is filled

        // Apply Hanning window to the time domain signal
        for (size_t i = 0; i < SAMPLE_SIZE; ++i)
        {
            time_domain_signal[i] = ((int32_t)time_domain_signal[i] * (int32_t)hanning_window[i]) >> 15;
        }

        // Perform FFT on the windowed time domain signal
        arm_rfft_q15(&fft_instance, time_domain_signal, freq_domain_signal);

        for (size_t pair_index = 0; pair_index < (SAMPLE_SIZE + 2) / 2; pair_index++)
        {
            int16_t real = freq_domain_signal[pair_index * 2];
            int16_t imag = freq_domain_signal[pair_index * 2 + 1];
            spectral_density[pair_index] = (uint64_t)real * real + (uint64_t)imag * imag;
            // printf("real: %d, imag: %d, spectral density: %llu\n", real, imag, spectral_density[pair_index]);
        }

        uint16_t frequency_bin_sums[12] = {0};
        uint16_t max_bin_sum = 0;
        size_t freq_bin_boundaries[13] = {0, 8, 11, 16, 24, 35, 51, 75, 110, 161, 237, 349, 512};
        //                                |--|---|---|---|---|---|---|---|----|----|----|----|
        //                                 0   1   2    3  4   5   6   7    8    9   10   11
        // Iterate through each frequency bin to calculate the sum of spectral densities
        for (size_t bin_index = 0; bin_index < 12; ++bin_index)
        {
            size_t bin_lower_boundary = freq_bin_boundaries[bin_index];
            size_t bin_upper_boundary = freq_bin_boundaries[bin_index + 1];
            uint32_t bin_sum = 0;

            for (size_t i = bin_lower_boundary; i < bin_upper_boundary; ++i)
            {
                bin_sum += spectral_density[i];
            }

            frequency_bin_sums[bin_index] = bin_sum;

            // Track the maximum bin sum for scaling
            if (bin_sum > max_bin_sum)
            {
                max_bin_sum = bin_sum;
            }
        }

        // Scale the frequency bin values to uint8_t (0 to 255) based on the maximum value
        uint8_t scaled_frequency_bin_sums[12] = {0};
        if (max_bin_sum > 0) // Ensure there is a non-zero value to scale
        {
            for (size_t i = 0; i < 12; ++i)
            {
                // Scale each bin sum to fit in the range 0-255
                scaled_frequency_bin_sums[i] = static_cast<uint8_t>((frequency_bin_sums[i] * 255) / max_bin_sum);
            }
        }
        int my_number = 0;
        // Update LED colors based on the frequency bin sums
        for (size_t bin_index = 0; bin_index < 12; ++bin_index)
        {
            // Create a color based on the power in the bin and set the corresponding LED color
            colour bin_colour = base_colour;

            uint8_t scaled_value = scaled_frequency_bin_sums[bin_index]; // Use the correct array
            bin_colour.set_hue(scaled_value);
            bin_colour.set_value(scaled_value);
            leds.set_colour_individual(my_number, bin_colour); // Set the color for the corresponding LED
            my_number++;
        }

        // Clear LEDs when the task is finished
        leds.clear_all();
    }
}