#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define SPI_TFT_SCK 22
#define SPI_TFT_CSn 21
#define SPI_TFT_TX 23



void init_chardisp_pins() {
    // fill in    
    gpio_set_function(SPI_TFT_SCK,GPIO_FUNC_SPI);
    gpio_set_function(SPI_TFT_TX,GPIO_FUNC_SPI);
    gpio_set_function(SPI_TFT_CSn,GPIO_FUNC_SPI);
    spi_init (spi0, 10000); // Always enable DREQ signals -- harmless if DMA is not listening
    //low clock frequency to make sure the display has enough time to process commands sent to it
    // will increase if transmitted signal requires higher freq
    spi_set_format(spi0,9,0,0,SPI_MSB_FIRST);
    //9 bits for LCD - verify
}

void send_spi_cmd(spi_inst_t* spi, uint16_t value){
    while (spi_is_busy(spi0));
    spi_write16_blocking(spi0,&value,1);
}
void send_spi_data(spi_inst_t* spi, uint16_t value){
    send_spi_cmd(spi0,value|0x100);
}

void cd_init(){
    sleep_ms(1);
    //Function Set command 0b0000101100
     //Display On/Off command 0b0000001100
    //Clear Display 0b0000000001
     //Entry Mode Set command 0b0000000110
}