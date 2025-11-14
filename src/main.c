#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <math.h>   

/****************************************** */
#define PIN_SDI    15 //TX
#define PIN_CS     13
#define PIN_SCK    14
#define PIN_DC     12
#define PIN_nRESET 16

// Uncomment the following #define when 
// you are ready to run Step 3.

// WARNING: The process will take a VERY 
// long time as it compiles and uploads 
// all the image frames into the uploaded 
// binary!  Expect to wait 5 minutes.
//#define ANIMATION
//#define PROJECT
/****************************************** */
#ifdef PROJECT
void run_oscilloscope();
#endif

#ifdef ANIMATION
#include "images.h"
#endif
/****************************************** */

void init_spi_lcd() {
    gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
    gpio_set_function(PIN_DC, GPIO_FUNC_SIO);
    gpio_set_function(PIN_nRESET, GPIO_FUNC_SIO);

    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_nRESET, GPIO_OUT);

    gpio_put(PIN_CS, 1); // CS high
    gpio_put(PIN_DC, 0); // DC low
    gpio_put(PIN_nRESET, 1); // nRESET high

    // initialize SPI1 with 48 MHz clock
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SDI, GPIO_FUNC_SPI);
    spi_init(spi1, 20 * 1000 * 1000);
    spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

Picture* load_image(const char* image_data);
void free_image(Picture* pic);

int main() {
    
    stdio_init_all();

    init_spi_lcd();

    LCD_Setup();
    LCD_Clear(0x0000); // Clear the screen to black
    #ifdef PROJECT
        run_oscilloscope();
        LCD_DrawString(10, 5, 0xF800, 0x0000, "DIV: 20ms", 5,0);
        LCD_DrawString(10, 25, 0xF800, 0x0000,"CH1", 5,0);
        LCD_DrawString(150, 5, 0xF800, 0x0000, "1V/DIV", 5,0);
    #endif
    #ifndef ANIMATION
    #define N_BODIES   3      // Number of bodies in the simulation
    #define G          12.0f  // Gravitational constant
    #define DT         0.01f // Simulation time step
    #define SOFTENING  5.0f   // Prevents extreme forces at close range

    // Colors as per the 16-bit RGB565 specification.
    #define BLACK      0x0000
    #define RED        0xF800
    #define LIME       0x07E0   // brighter green
    #define BLUE       0x001F
    // Oscilloscope settings
    #define GRID_COLOR 0x7BEF   // light gray
    #define TRACE_COLOR 0xF800  // red
    #define BACKGROUND 0x0000   // black
    #define LCD_W 240
    #define LCD_H 320


    // Make things easier to keep track of for each "body".
    typedef struct {
        float x, y, vx, vy, mass;
        uint16_t color;
    } Body;

    // Clear everything so we start from scratch
    LCD_Clear(BLACK);
    //GRID
    for (int x = 20; x < LCD_W; x += 20) {
        for (int y = 20; y < LCD_H; y++) {
            LCD_DrawPoint(x, y, GRID_COLOR);
        }
    }

    // Horizontal grid every 20 pixels
    for (int y = 0; y < LCD_H; y += 20) {
        for (int x = 0; x < LCD_W; x++) {
            LCD_DrawPoint(x, y, GRID_COLOR);
        }
    }
    char *vDIV = "1V/DIV";
    LCD_DrawString(20, 5, 0xF800, 0x0000, vDIV, 12,1); //"1V/DIV"

    char *sDIV = "1s/DIV";
    LCD_DrawString(100, 5, 0xF800, 0x0000, sDIV, 12,1); //"1V/DIV"
    // Initialize all bodies in a compact list
    Body bodies[N_BODIES] = {
        { .x=120.0f, .y=100.0f, .vx= 1.2f, .vy= 0.5f, .mass=20.0f, .color=RED  },
        { .x=180.0f, .y=250.0f, .vx=-0.8f, .vy=-1.0f, .mass=25.0f, .color=LIME },
        { .x= 60.0f, .y=250.0f, .vx= 0.5f, .vy= 0.9f, .mass=30.0f, .color=BLUE }
    };

    float get_sample(float t) {
        // SAMPLE SIGNAL
        // You can replace this with ADC input later.
        return 0.5f * sinf(2 * 3.14159f * 2.0f * t)   // 2 Hz wave
            + 0.25f * sinf(2 * 3.14159f * 10.0f * t) // 10 Hz wave
            ;
    }
    
    // // Infinite Animation Loop
    // while(1) {
    //     // Calculate accelerations and update velocities
    //     for (int i = 0; i < N_BODIES; i++) {
    //         float total_accel_x = 0.0f;
    //         float total_accel_y = 0.0f;

    //         for (int j = 0; j < N_BODIES; j++) {
    //             if (i == j) continue;

    //             float dx = bodies[j].x - bodies[i].x;
    //             float dy = bodies[j].y - bodies[i].y;
    //             // d^2 = dx^2 + dy^2 (+ a fake softening factor to avoid collisions)
    //             float dist_sq = dx * dx + dy * dy + SOFTENING;
    //             // Newton's law of gravitation: F = G * m1 * m2 / d^2
    //             float inv_dist_cubed = 1.0f / (dist_sq * sqrtf(dist_sq));
                
    //             // Acceleration = Force / mass, but we multiply by mass to get the force directly
    //             // so we can use it to update velocity directly.
    //             total_accel_x += dx * inv_dist_cubed * bodies[j].mass * G;
    //             total_accel_y += dy * inv_dist_cubed * bodies[j].mass * G;
    //         }
    //         bodies[i].vx += total_accel_x * DT;
    //         bodies[i].vy += total_accel_y * DT;
    //     }
        
    //     // Update positions and draw each body
    //     for (int i = 0; i < N_BODIES; i++) {
    //         // new position = old position + velocity * time step
    //         bodies[i].x += bodies[i].vx * DT;
    //         bodies[i].y += bodies[i].vy * DT;

    //         // Wrap around screen edges
    //         if (bodies[i].x < 0)    bodies[i].x += 240;
    //         if (bodies[i].x >= 240) bodies[i].x -= 240;
    //         if (bodies[i].y < 0)    bodies[i].y += 320;
    //         if (bodies[i].y >= 320) bodies[i].y -= 320;

    //         LCD_DrawPoint((uint16_t)bodies[i].x, (uint16_t)bodies[i].y, bodies[i].color);
    //     }

    //     // Slow it WAY down so we can see the planets interact with each other!
    //     sleep_ms(1);
    // }
    // #endif
    
    /*
        Now, for some more fun!

        Uncomment the ANIMATION #define at the top of main.c 
        to run this section.
        
        We've converted a popular GIF into a series of images, 
        and stored each of those frames in its own C array.  
        Look at the lab for the script we wrote to do this.

        This is an example of how you can draw a very large picture, 
        but notice how slow the animation is, even at 100 MHz.  
        The LCD_DrawPicture function is not really intended for such 
        large images, but it will be very helpful for smaller ones, 
        like scary monsters and nice sprites in a game.
    */ 

    #ifdef ANIMATION
    Picture* frame_pic = NULL;
    int frame_index = 0;
    while (1) { // Loop forever
        // Get the next frame from the array
        frame_pic = load_image(mystery_frames[frame_index]);
    
        if (frame_pic) {
            // Draw the frame to the top-left corner of the screen
            LCD_DrawPicture(0, 0, frame_pic);
            
            // Free the Picture struct (not the pixel data)
            free_image(frame_pic);
        }
    
        // Move to the next frame, looping back to the start
        frame_index++;
        if (frame_index >= mystery_frame_count) {
            frame_index = 0;
        }
    
        // Add a small delay to control animation speed
        sleep_ms(1); // Adjust delay as needed
    }
    #endif

    for(;;);
}

#ifdef PROJECT
    #define LCD_W 240
    #define LCD_H 320

    // Oscilloscope settings
    #define GRID_COLOR 0x7BEF   // light gray
    #define TRACE_COLOR 0xF800  // red
    #define BACKGROUND 0x0000   // black

    void draw_grid() {
        // Clear the screen
        LCD_Clear(BACKGROUND);

        // Vertical grid every 20 pixels
        for (int x = 0; x < LCD_W; x += 20) {
            for (int y = 0; y < LCD_H; y++) {
                LCD_DrawPoint(x, y, GRID_COLOR);
            }
        }

        // Horizontal grid every 20 pixels
        for (int y = 0; y < LCD_H; y += 20) {
            for (int x = 0; x < LCD_W; x++) {
                LCD_DrawPoint(x, y, GRID_COLOR);
            }
        }
    }


    float get_sample(float t) {
        // SAMPLE SIGNAL
        // You can replace this with ADC input later.
        return 0.5f * sinf(2 * 3.14159f * 2.0f * t)   // 2 Hz wave
            + 0.25f * sinf(2 * 3.14159f * 10.0f * t) // 10 Hz wave
            ;
    }

    void run_oscilloscope() {
        draw_grid();

        int x = 0;
        float t = 0;
        const float dt = 0.01f;

        int last_y = LCD_H / 2;

        while (1) {
            // Get a sample
            float v = get_sample(t);
            t += dt;

            // Convert voltage to pixel height
            // v ∈ [-1,1] → y ∈ [0,319]
            int y = (int)((1.0f - v) * (LCD_H / 2));

            // Draw trace point
            LCD_DrawPoint(x, y, TRACE_COLOR);

            // Erase behind the point (scrolling effect)
            int old_x = (x + 1) % LCD_W;
            for (int yy = 0; yy < LCD_H; yy++) {
                LCD_DrawPoint(old_x, yy, BACKGROUND);
            }

            // Redraw grid lines on top
            if (old_x % 20 == 0 || y % 20 == 0)
                LCD_DrawPoint(old_x, y, GRID_COLOR);

            // Move x coordinate
            x++;
            if (x >= LCD_W) {
                x = 0;
            }

            sleep_ms(1);
        }
    }

    #endif