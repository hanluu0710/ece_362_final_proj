#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <math.h>  
#define _USE_MATH_DEFINES 
#include "wavegen.h"
#include "buttons.h"




#define BUFFER_SIZE 4096
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
#define VIRTUAL_0 0xFF00// yellow

#define SINETEST
void init_adc_dma(void);  // Declare the function
void init_encoders(void);
uint16_t adc_get_sample();
uint16_t adc_get_next_sample();


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

int volts_per_div = 1;
int time_per_div = 1;
int pixels_per_div = 20; //20-LCH-20
float dt = 0.001; // Xms/div
bool grid_dirty = false;
bool pause = false;
extern int x_mode;
extern int y_mode;
//grid and general display for buttons
void draw_grid() {
    LCD_Clear(BACKGROUND);

    // draw grids
    for (int xx = 0; xx < LCD_W; xx++) {
        for (int yy = 20; yy < LCD_H-20+1; yy++) {
            if ((xx % 20 == 0 || yy % 20 == 0)) {
                LCD_DrawPoint(xx+12, yy, GRID_COLOR);
            }
            if ((yy== 160-(y_offset*pixels_per_div))){
                LCD_DrawPoint(xx+12, yy%(LCD_H), VIRTUAL_0);
            }
        }
    }
    char bufVolt[20];
    char bufTime[20];
    sprintf(bufVolt, "%dV/DIV", volts_per_div);
    sprintf(bufTime, "%dms/DIV", time_per_div);
    LCD_DrawString(20, 5, 0xF800, BACKGROUND, bufVolt, 12, 1);
    LCD_DrawString(LCD_W-60, 5, 0xF800, BACKGROUND, bufTime, 12, 1);
    
    //ALL labels stay the same, only virtual 0 line moves to show how much is offsetted
    char buf[8];
    for (int i = 0; i <= LCD_H/2-20; i += pixels_per_div){
        int label_voltage = volts_per_div * (i / pixels_per_div);
        sprintf(buf, "%d", label_voltage);
        LCD_DrawString(0, (LCD_H/2) - i-5, 0xF800, BACKGROUND, buf, 12, 1);
        LCD_DrawString(0, (LCD_H/2) + i-5, 0xF800, BACKGROUND, buf, 12, 1);
    }
    for (int i = 10; i <= LCD_W-30; i += pixels_per_div){
        int label_time = time_per_div * ((i-10) / pixels_per_div);
        sprintf(buf, "%d", label_time);
        LCD_DrawString(i, LCD_H - 15, 0xF800, BACKGROUND, buf, 12, 1);
    }
}
float get_sample(float t) { // SAMPLE SIGNAL 
    // You can replace this with ADC input later. 
    return 2 * sin(2* 3.14159f *100*t); // 2 Hz wave + 0.25f * sinf(2 * 3.14159f * 10.0f * t) // 10 Hz wave ; }
}
void display_mode_message(const char *msg) {
    LCD_DrawFillRectangle(LCD_W/2-50,5,LCD_W/2+40,19,BACKGROUND);
    LCD_DrawString(LCD_W/2-20, 5, 0xF800, BACKGROUND, msg, 12, 1);
}

#ifdef SINETEST
void run_oscilloscope() {
    draw_grid();

    int x = 0;
    float t = 0;
    float volt1 = 0;
    float volt2 = 0;
    int old_x =0;
    float pixel1[LCD_W]={0};
    float pixel2[LCD_W]={0};
    int pixel_per_volt = 0;

    while (1) {
        controls();
        if (pause){
            sleep_ms(5);
            continue;
        }
        if (x_mode_changed) {
            x_mode_changed = false;

            char msg[16];
            sprintf(msg, "%s X", x_mode ? "Shift" : "Scale");
            display_mode_message(msg);
        }

        if (y_mode_changed) {
                y_mode_changed = false;

                char msg[16];
                sprintf(msg, "%s Y", y_mode ? "Shift" : "Scale");
                display_mode_message(msg);
        }

        pixel_per_volt = pixels_per_div/volts_per_div;
        float time_per_pixel = (float)time_per_div / pixels_per_div;
        if (grid_dirty) {
            draw_grid();
            grid_dirty = false;
            char msg[16];
            sprintf(msg, "%s X", x_mode ? "Shift" : "Scale");
            display_mode_message(msg);
            sprintf(msg, "%s Y", y_mode ? "Shift" : "Scale");
            display_mode_message(msg);
        }
         for (int yy = 20; yy < LCD_H - 20+1; yy++) {
            if ((x % 20 == 0 || yy % 20 == 0)) {
                LCD_DrawPoint(x+12, yy, GRID_COLOR);
            }
            else{
                LCD_DrawPoint(x+12, yy, BACKGROUND);
            }
            if ((yy== 160-y_offset*pixels_per_div)){
                LCD_DrawPoint(x+12, yy, VIRTUAL_0);
            }
        }
        // sample the signal
        volt1 = get_sample(t);
        t += time_per_pixel * 0.001; //t += dt;
        volt2 = get_sample(t);

        // convert to vertical pixel
        pixel1[x] = ((LCD_H) / 2) - volt1 * pixel_per_volt - y_offset*pixels_per_div;
        pixel2[x] = ((LCD_H) / 2) - volt2* pixel_per_volt - y_offset*pixels_per_div;

        if (pixel1[x]<pixels_per_div){
            pixel1[x] = pixels_per_div;
        }
        if (pixel1[x]>LCD_H-pixels_per_div){
            pixel1[x] = LCD_H-pixels_per_div;
        }
        if (pixel2[x]<pixels_per_div){
            pixel2[x] = pixels_per_div;
        }
        if (pixel2[x]>LCD_H-pixels_per_div){
            pixel2[x] = LCD_H-pixels_per_div;
        }
        // draw line
        old_x = (x==0)? 0:x-1; // %240, always wrap around after reaching end of screen
        LCD_DrawLine(old_x+12,pixel1[old_x],x+12,pixel2[x],TRACE_COLOR);

        x = ((x+1) % (LCD_W)); // %240, always wrap around after reaching end of screen
        sleep_ms(2);
    }
}
#else
//update signal
void run_oscilloscope() {
    draw_grid();

    int x = 0;
    float volt = 0;
    int old_x =0;
    int y=0;
    int ybuf [LCD_W] = {0};
   // float running_offset = 1.65;   // assume mid-supply at start

    while (1) {
        for (int yy = 20; yy < LCD_H - 19; yy++) {
            if ((x % 20 == 0 || yy % 20 == 0)) {
                LCD_DrawPoint(x, yy, GRID_COLOR);
            }
            else{
                LCD_DrawPoint(x, yy, BACKGROUND);
            }
            if ((yy== 160)){
                LCD_DrawPoint(x, yy, VIRTUAL_0);
            }
        }
        // sample the signal
        volt = _next_sample();
        float norm_volt = volt * (3.3/4095.0); // assuming 12-bit ADC
        //int running_offset = 0.999 * running_offset + 0.001 * norm_volt;
        float center_volt =norm_volt;
        float volts_per_div = 0.5;      // each grid square is 0.5V
        float pixels_per_div = 20.0;    // each grid square is 20px
        float pixels = center_volt * (pixels_per_div / volts_per_div);

        printf("Sample: %.3f\n", volt);
        // convert to vertical pixel
        int y = pixels+160;
        if (y < 20){y = 20;}
        if (y>LCD_H-20){y = LCD_H-20;}
        ybuf[x] = y;
        // draw line
        old_x = (x==0)? 0:x-1; // %240, always wrap around after reaching end of screen
        LCD_DrawLine(old_x,ybuf[old_x],x,ybuf[x],TRACE_COLOR);

        x = (x+1) % (LCD_W-1); // %240, always wrap around after reaching end of screen

        sleep_ms(3);
    }
}
#endif


int main() {
    stdio_init_all();
    init_gpio();
    init_spi_lcd();
    init_adc_dma();
    LCD_Setup();
    LCD_Clear(BACKGROUND);
    init_encoders(); 
    run_oscilloscope();
    

    while (1);
}