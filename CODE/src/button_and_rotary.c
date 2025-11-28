#include "pico/stdlib.h"
#include <stdio.h>
#include <stdbool.h>
#include "buttons.h"

// //global var
// static bool last_mode_state = false;
// static bool last_capture_state = false;
// static bool last_x_state = false;
// static bool last_y_state = false;
// extern bool pause;

// #define WAVE_COUNT 5

// // scale settings of the rotary
// int x_scale = 0;
// int y_scale = 0;
// bool x_fast = false;
// bool y_fast = false;

void init_gpio()
{
    // Initializing BUTTON_MODE
    gpio_init(BUTTON_MODE);
    gpio_set_dir(BUTTON_MODE, false);

    // Initializing BUTTON_CAPTURE
    gpio_init(BUTTON_CAPTURE);
    gpio_set_dir(BUTTON_CAPTURE, false);

    // Initializing X encoder
    gpio_init(X_CLK);
    gpio_set_dir(X_CLK, GPIO_IN);
    gpio_pull_up(X_CLK);

    gpio_init(X_DT);
    gpio_set_dir(X_DT, GPIO_IN);
    gpio_pull_up(X_DT);

    gpio_init(X_SW);
    gpio_set_dir(X_SW, GPIO_IN);
    gpio_pull_up(X_SW);

    // Initializing Y encoder
    gpio_init(Y_CLK);
    gpio_set_dir(Y_CLK, GPIO_IN);
    gpio_pull_up(Y_CLK);

    gpio_init(Y_DT);
    gpio_set_dir(Y_DT, GPIO_IN);
    gpio_pull_up(Y_DT);

    gpio_init(Y_SW);
    gpio_set_dir(Y_SW, GPIO_IN);
    gpio_pull_up(Y_SW);

    printf("Wave Generator Ready\n");
}

// bool check_mode_button()
// {
//     bool current_state = gpio_get(BUTTON_MODE);
//     if (!last_mode_state && current_state)
//     {
//         last_mode_state = current_state;
//         sleep_ms(1000);
//         return true;
//     }
//     last_mode_state = current_state;
//     return false;
// }

// bool check_capture_button(){
//     int current_state = gpio_get(BUTTON_CAPTURE);
//     if (!last_capture_state && current_state)
//     {
//         last_capture_state = current_state;
//         return true;
//     }
//     last_capture_state = current_state;
//     return false;
// }

// int read_encoder_x()
// {
//     static int last_clk_x = 0;
//     int clk = gpio_get(X_CLK);
//     int dt = gpio_get(X_DT);
//     int result = 0;

//     if (clk != last_clk_x)
//     {                                  // pulse happended
//         result = (dt != clk) ? 1 : -1; // 1 = clockwise
//     }
//     last_clk_x = clk;
//     return result;
// }
// int read_encoder_y()
// {
//     static int last_clk_y = 1;
//     int clk = gpio_get(Y_CLK);
//     int dt = gpio_get(Y_DT);
//     int result = 0;
//     if (clk != last_clk_y)
//     {   
//         result = (dt != clk) ? 1 : -1; // pulse happended
//     }
//     last_clk_y = clk;
//     return result;
// }

// bool check_x_button()
// {
//     bool current = gpio_get(X_SW);

//     if (!last_x_state && current)
//     {
//         sleep_ms(100);
//         last_x_state = current;
//         return true;
//     }
//     last_x_state = current;
//     return false;
// }


// bool check_y_button()
// {
//     bool current = gpio_get(Y_SW);

//     if (!last_y_state && current)
//     {
//         sleep_ms(100);
//         last_y_state = current;
//         return true;
//     }
//     last_y_state = current;
//     return false;
// }

// // void wave_output(wave_type wave)
// // {
// //     switch (wave)
// //     {
// //     case WAVE_OFF:
// //         printf("Wave OFF\n");
// //         break;
// //     case WAVE_SINE:
// //         printf("Wave SINE\n");
// //         break;
// //     case WAVE_SAWTOOTH:
// //         printf("Wave SAW\n");
// //         break;
// //     case WAVE_TRIANGLE:
// //         printf("Wave TRIANGLE\n");
// //         break;
// //     case WAVE_SQUARE:
// //         printf("Wave SQUARE\n");
// //         break;
// //     default:
// //         break;
// //     }
// // }





// // read the x scale encoder
// // int read_encoder_x() {
// //     static int last_clk = 1;
// //     int clk = gpio_get(X_CLK);

// //     if (clk != last_clk && clk == 0) {  // detect falling edge
// //         if (gpio_get(X_DT) == 1)
// //             return +1;   // clockwise
// //         else
// //             return -1;   // counter-clockwise
// //     }
// //     last_clk = clk;
// //     return 0;
// // }



// // y scale encoder reading
// // int read_encoder_y()
// // {
// //     static int last_clk = 1;
// //     int clk = gpio_get(Y_CLK);

// //     if (clk != last_clk && clk == 0)
// //     { // detect falling edge
// //         if (gpio_get(Y_DT) == 1)
// //             return +1; // clockwise
// //         else
// //             return -1; // counter-clockwise
// //     }
// //     last_clk = clk;
// //     return 0;
// // }

// // check when x rotray button is pressed



// // this is check when y button is pressed or not

// int x_mode = 0;  // 0 = scale, 1 = shift
// int y_mode = 0;

// void controls()
// {
//     static int current_wave = 0;
//     extern int volts_per_div;
//     extern int time_per_div;
//     extern float dt;
//     extern bool grid_dirty;
//     extern bool pause;

//     float x_step = 0.3;
//     float y_step = 0.3;

//     float x_offset = 0;
//     float y_offset = 0;


//     //wave_output(current_wave);
//     // Initializing scaling steps and initial scales
//     // float x_scale = 1.0f;
//     // float y_scale = 1.0f;
//     // float x_offset = 0.0f;
//     // float y_offset = 0.0f;

//     // For the push buttons
//     if (check_mode_button())
//     {
//         // if (gpio_get(BUTTON_MODE)) {
//         current_wave = (current_wave + 1) % WAVE_COUNT;
//         printf("Wave changed to %d\n", current_wave);
//     }

//     if (check_capture_button())
//     {
//         printf("Successfully Captured Waveform! :D\n");
//     }
//     // For the rotary encoder
//     // to check mode of the buttons
//     if (check_x_button())
//     {
//         x_mode = !x_mode;
//         char *x_check_mode = x_mode ? "Shift" : "Scale";
//         printf("X mode: %s\n", x_check_mode);
//     }

//     if (check_y_button())
//     {
//         y_mode = !y_mode;
//         char *y_check_mode = y_mode ? "Shift" : "Scale";
//         printf("Y mode: %s\n", y_check_mode);
//     }

//     // check what mode it is and scale or shift accordingly
//     int x_check_turn = read_encoder_x();
//     int y_check_turn = read_encoder_y();
//     if (x_check_turn != 0)
//     {
//         if (x_mode)
//         { // 1 = shift, 0 = scale
//             if (x_check_turn > 0)
//             {
//                 x_offset += x_step;
//             }
//             else
//             {
//                 x_offset -= x_step;
//             }
//             printf("X offset = %.2f\n", x_offset);
//         }
//         else{
//             if (x_check_turn > 0){
//                 time_per_div++;
//             }
//             else{
//                 time_per_div--;
//                 if (time_per_div < 1){
//                     time_per_div = 1;
//                 }
//             }
//             printf("Time div = %dms\n", time_per_div);
//             dt = time_per_div * 0.001;
//             grid_dirty = true;
//         }
//     }

//     if (y_check_turn != 0){
//         if (y_mode){
//             if (y_check_turn > 0){
//                 y_offset += y_step;
//             }
//             else{
//                 y_offset -= y_step;
//             }
//             printf("Y offset = %.2f\n", y_offset);
//         }
//         else{
//             if (y_check_turn > 0){
//                 volts_per_div++;
//             }
//             else{
//                 volts_per_div--;
//                 if (y_scale < 1){
//                     volts_per_div = 1;
//                 }
//             }
//             printf("Y scale = %dV\n", volts_per_div);
//             grid_dirty = true;
//         }
//     }
//     // sleep_ms(10);
// }

// ---- Encoder Counts (from interrupt) ----
volatile int x_encoder_steps = 0;
volatile int y_encoder_steps = 0;

// ---- Encoder Modes ----
int x_mode = 0;  // 0 = scale, 1 = shift
int y_mode = 0;
bool x_mode_changed = false;
bool y_mode_changed = false;


// ---- Shift and Scale Variables ----
float x_offset = 0;
float y_offset = 0;

extern int volts_per_div;
extern int time_per_div;
extern float dt;
extern bool grid_dirty;

void encoder_callback(uint gpio, uint32_t events)
{
    if (gpio == X_CLK)
    {
        int clk = gpio_get(X_CLK);
        int dt  = gpio_get(X_DT);
        x_encoder_steps += (dt != clk) ? 1 : -1;
        x_encoder_steps %= 10;
    }
    else if (gpio == Y_CLK)
    {
        int clk = gpio_get(Y_CLK);
        int dt  = gpio_get(Y_DT);
        y_encoder_steps += (dt != clk) ? 1 : -1;
    }
}

void init_encoders()
{
    // X encoder
    gpio_init(X_CLK);
    gpio_set_dir(X_CLK, GPIO_IN);
    gpio_pull_up(X_CLK);

    gpio_init(X_DT);
    gpio_set_dir(X_DT, GPIO_IN);
    gpio_pull_up(X_DT);

    // Y encoder
    gpio_init(Y_CLK);
    gpio_set_dir(Y_CLK, GPIO_IN);
    gpio_pull_up(Y_CLK);

    gpio_init(Y_DT);
    gpio_set_dir(Y_DT, GPIO_IN);
    gpio_pull_up(Y_DT);

    gpio_set_irq_enabled_with_callback(
    X_CLK,
    GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
    true,
    &encoder_callback
    );

    gpio_set_irq_enabled(
        Y_CLK,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true
    );

}

bool check_button(int pin, bool *last_state)
{
    bool s = gpio_get(pin);
    if (s && !(*last_state))
    {
        sleep_ms(20);  // debounce
        *last_state = true;
        return true;
    }
    if (!s)
        *last_state = false;

    return false;
}

bool last_x_button = false;
bool last_y_button = false;
bool last_mode_button = false;
bool last_capture_button = false;

void controls()
{
    // ---- Handle X Button (toggle mode) ----
    if (check_button(X_SW, &last_x_button))
    {
        x_mode = !x_mode;
        x_mode_changed = true;
        printf("X Mode: %s\n", x_mode ? "Shift" : "Scale");
    }

    // ---- Handle Y Button (toggle mode) ----
    if (check_button(Y_SW, &last_y_button))
    {
        y_mode = !y_mode;
        y_mode_changed = true;
        printf("Y Mode: %s\n", y_mode ? "Shift" : "Scale");
    }

    // ---- MODE button (wave change) ----
    if (check_button(BUTTON_MODE, &last_mode_button)){
        printf("Mode button pressed\n");
    }
    // ---- CAPTURE button ----
    if (check_button(BUTTON_CAPTURE, &last_capture_button)){
        printf("Capture pressed\n");
    }

    // ---- Consume X encoder steps ----
    int steps_x = x_encoder_steps;
    x_encoder_steps = 0;

    if (steps_x != 0)
    {
        if (x_mode == 1)        // SHIFT
        {
            x_offset += steps_x * 0.3f;
            printf("X Offset = %.2f\n", x_offset);
            grid_dirty = true;
        }
        else                    // SCALE (time)
        {
            time_per_div += steps_x;
            time_per_div %= 11;
            if (time_per_div < 1) time_per_div = 1;

            dt = time_per_div * 0.001; // second
            grid_dirty = true;

            printf("Time/div = %dms\n", time_per_div);
        }
    }

    // ---- Consume Y encoder steps ----
    int steps_y = y_encoder_steps;
    y_encoder_steps = 0;

    if (steps_y != 0)
    {
        if (y_mode == 1)       // SHIFT
        {
            y_offset = y_offset + steps_y * volts_per_div;
            grid_dirty = true;
            printf("Y Offset = %.2f\n", y_offset);
        }
        else                   // SCALE (voltage)
        {
            volts_per_div += steps_y;
            volts_per_div %= 11;
            if (volts_per_div < 1) volts_per_div = 1;

            grid_dirty = true;
            printf("Volts/div = %dV\n", volts_per_div);
        }
    }
}
