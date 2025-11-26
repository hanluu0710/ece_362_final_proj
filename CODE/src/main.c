#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <math.h>   
#include "wavegen.h"

// TFT pins
#define PIN_SDI    15
#define PIN_CS     13
#define PIN_SCK    14
#define PIN_DC     12
#define PIN_nRESET 16

//LCD const
#define LCD_W 240
#define LCD_H 320
#define GRID_COLOR 0x7BEF // blue
#define TRACE_COLOR 0xF800 // neon green
#define BACKGROUND 0x0000 // black

void init_adc_dma(void);  // Declare the function
float adc_get_sample (float);

void init_spi_lcd() {
    gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
    gpio_set_function(PIN_DC, GPIO_FUNC_SIO);
    gpio_set_function(PIN_nRESET, GPIO_FUNC_SIO);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_nRESET, GPIO_OUT);

    gpio_put(PIN_CS, 1);
    gpio_put(PIN_DC, 0);
    gpio_put(PIN_nRESET, 1);

    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SDI, GPIO_FUNC_SPI);

    spi_init(spi1, 20 * 1000 * 1000);
    spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

//grid and general display for buttons
void draw_grid() {
    LCD_Clear(BACKGROUND);

    // draw grids
    for (int xx = 0; xx < LCD_W; xx++) {
        for (int yy = 20; yy < LCD_H-19; yy++) {
            if (xx % 20 == 0 || yy % 20 == 0) {
                LCD_DrawPoint(xx, yy, GRID_COLOR);
            }
        }
    }

    LCD_DrawString(5, 5, 0xF800, BACKGROUND, "1V/DIV", 12, 1);
    LCD_DrawString(100, 5, 0xF800, BACKGROUND, "20ms/DIV", 12, 1);
    LCD_DrawString(10, LCD_H - 15, 0xF800, BACKGROUND, "Time Scale", 12, 1);
}
float get_sample(float t) { // SAMPLE SIGNAL 
    // You can replace this with ADC input later. 
    return 0.5f * sinf(2 * 3.14159f * 2.0f * t); // 2 Hz wave + 0.25f * sinf(2 * 3.14159f * 10.0f * t) // 10 Hz wave ; }
}

//update signal
void run_oscilloscope() {
    draw_grid();

    int x = 0;
    float t = 0;
    float dt = 0.01;
    float volt1 = 0;
    float volt2 = 0;
    int old_x =0;
    int y1[LCD_W]={0};
    int y2[LCD_W]={0};

    while (1) {
        for (int yy = 20; yy < LCD_H - 19; yy++) {
            if (x % 20 == 0 || yy % 20 == 0)
                LCD_DrawPoint(x, yy, GRID_COLOR);
            else
                LCD_DrawPoint(x, yy, BACKGROUND);
        }
        // sample the signal
        volt1 = adc_get_sample(t);
        t += dt;
        volt2 = adc_get_sample(t);

        // convert to vertical pixel
        y1[x] = (int)((1.0 - volt1) * ((LCD_H) / 2));
        y2[x] = (int)((1.0 - volt2) * ((LCD_H) / 2));

        // draw line
        old_x = (x==0)? 0:x-1; // %240, always wrap around after reaching end of screen
        LCD_DrawLine(old_x,y1[old_x],x,y2[x],TRACE_COLOR);

        x = (x+1) % (LCD_W-1); // %240, always wrap around after reaching end of screen

        sleep_ms(10);
    }
}

int main() {
    stdio_init_all();

    init_spi_lcd();
    init_adc_dma();
    LCD_Setup();
    LCD_Clear(BACKGROUND);

    run_oscilloscope();
    while (1);
}