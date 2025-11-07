#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define SPI_TFT_SCK 22
#define SPI_TFT_CSn 21
#define SPI_TFT_TX 23



void display_init_spi() {
    // fill in    
    gpio_set_function(SPI_TFT_SCK,GPIO_FUNC_SPI);
    gpio_set_function(SPI_TFT_TX,GPIO_FUNC_SPI);
    gpio_set_function(SPI_TFT_CSn,GPIO_FUNC_SPI);
    spi_init (spi1, 125000); // Always enable DREQ signals -- harmless if DMA is not listening
    spi_set_format(spi1,16,0,0,SPI_MSB_FIRST);
}