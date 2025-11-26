#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////

// BUFFER_SIZE must be a power of 2. This is the number of samples in the buffer, not bytes
#define BUFFER_SIZE 4096 //= 32768 bytes / 8; Max size is 16384 samples = 32768 bytes / 2
uint16_t adc_fifo_out[BUFFER_SIZE] __attribute__((aligned(BUFFER_SIZE * 2)));

//////////////////////////////////////////////////////////////////////////////

void init_adc_freerun() {
    adc_init();
    adc_gpio_init(40);
    adc_select_input(0);
    adc_run(true);
}

void init_dma() {
    dma_channel_set_read_addr(0, &adc_hw->fifo, false);
    dma_channel_set_write_addr(0, &adc_fifo_out, false);
    dma_channel_hw_addr(0)->transfer_count = 0x10000001;
    
    /*
    for (int i = 0; i < BUFFER_SIZE; i++) {
        adc_fifo_out[i] = i;
    }
    */

    uint32_t temp;
    temp = (48 << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB)           // Select ADC as TREQ source
        |  (1 << DMA_CH0_CTRL_TRIG_RING_SEL_LSB)            // Ring select (write address increments)
        |  (__builtin_ctz(BUFFER_SIZE * 2)
            << DMA_CH0_CTRL_TRIG_RING_SIZE_LSB)             // Ring size (2^15 bytes)
        |  (1 << DMA_CH0_CTRL_TRIG_INCR_WRITE_LSB)          // Increment address after write
        |  (1 << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB)           // Select halfword (16 bits)
        |  (1 << DMA_CH0_CTRL_TRIG_EN_LSB);                 // Enable
    dma_hw->ch[0].ctrl_trig = temp;
}

void init_adc_dma() {
    init_dma();
    init_adc_freerun();
    adc_fifo_setup(true, true, 1, false, false);
}

uint16_t adc_get_sample(uint32_t i)
{
        return 0.5f * sinf(2 * 3.14159f * 2.0f * i); // 2 Hz wave + 0.25f * sinf(2 * 3.14159f * 10.0f * t) // 10 Hz wave ; }
    // // Convert base pointer to byte pointer
    // uint8_t *base = (uint8_t *)adc_fifo_out;

    // // Read DMA next-write address
    // uint32_t next = dma_hw->ch[0].write_addr;

    // // Compute byte offset of the i-th most recent sample
    // // -2 for most recent
    // // -(i * 2) for arbitrary sample i
    // uint32_t offset = (next - (uint32_t)base - 2 - (i * 2));    // Move back 1 + i samples
    // offset &= ((BUFFER_SIZE * 2) - 1);                          // Wrap around if necessary

    // // Convert back to uint16_t pointer and return the sample
    // return *(uint16_t *)(base + offset);
}

//////////////////////////////////////////////////////////////////////////////

// int main()
// {
//     // Configures our microcontroller to 
//     // communicate over UART through the TX/RX pins
//     stdio_init_all();

//     init_adc_dma();

//     uint16_t recent[10];
//     for(;;) {
//         // 0 = latest sample
//         // 1 = second latest sample
//         // etc
//         for (int i = 0; i < 10; i++) {
//             recent[i] = adc_get_sample(i);
//         }
//     }

//     for(;;);
//     return 0;
// }
