#include "microphone_task.h"
#include <stdio.h>

// Define the buffer size for the DMA buffer
#define CAPTURE_DEPTH 1024
uint16_t capture_buf[CAPTURE_DEPTH]; // Buffer to store samples

/*! \brief Function to run the microphone task, reading samples from the DMA buffer.
 *
 * This function initializes the microphone, then continuously reads audio samples from
 * the DMA buffer in a blocking fashion until `stop_task` is set to true.
 */
int run_microphone_task()
{
    // Create an instance of the microphone class
    microphone mic;

    // Initialize the microphone
    mic.init(26);

    while (true)
    {
        // Read from the microphone (blocking until buffer is filled)
        mic.read_blocking(capture_buf, CAPTURE_DEPTH);

        // Process the buffer (print each datapoint on a new line)
        for (int i = 0; i < CAPTURE_DEPTH; ++i)
        {
            printf("%d\n", capture_buf[i]); // Print each sample followed by a newline
        }
    }
}