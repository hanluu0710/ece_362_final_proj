#include "pico/stdlib.h"
#include <stdio.h>
#include <stdbool.h>

#define BUTTON_MODE 3
#define BUTTON_CAPTURE 4

#define X_CLK 5
#define X_DT 6
#define X_SW 7

#define Y_CLK 8
#define Y_DT 9
#define Y_SW 10

// scale settings of the rotary
int x_scale = 0;
int y_scale = 0;
bool x_fast = false;
bool y_fast = false;


typedef enum {
    WAVE_OFF,
    WAVE_SINE,
    WAVE_SAWTOOTH,
    WAVE_TRIANGLE,
    WAVE_SQUARE,
    WAVE_COUNT
} wave_type;

void wave_output(wave_type wave) {
    switch(wave) {
        case WAVE_OFF:
            printf("Wave OFF\n");
            break;
        case WAVE_SINE:
            printf("Wave SINE\n");
            break;
        case WAVE_SAWTOOTH:
            printf("Wave SAW\n");
            break;
        case WAVE_TRIANGLE:
            printf("Wave TRIANGLE\n");
            break;
        case WAVE_SQUARE:
            printf("Wave SQUARE\n");
            break;
        default:
            break;
    }
}

bool check_mode_button() {
    static bool last_state = false;
    bool current_state = gpio_get(BUTTON_MODE);

    if (!last_state && current_state) {
        last_state = current_state;
        sleep_ms(100);
        return true;
    }
    last_state = current_state;
    return false;
}

bool check_capture_button() {
    static bool last_state = false;
    bool current_state = gpio_get(BUTTON_CAPTURE);

    if (!last_state && current_state) {
        last_state = current_state;
        sleep_ms(100);
        return true;
    }
    last_state = current_state;
    return false;
}

// read the x scale encoder
int read_encoder_x() {
    static int last_clk_x = 0;
    int clk = gpio_get(X_CLK);
    int dt = gpio_get(X_DT);
    int result = 0;

    if (clk != last_clk_x) {
        result = (dt != clk) ? 1 : -1;
    }
    last_clk_x = clk;
    return result;
}

// y scale encoder reading
int read_encoder_y() {
    static int last_clk = 0;
    int clk = gpio_get(Y_CLK);
    int dt = gpio_get(Y_DT);
    int result = 0;

    if (clk != last_clk) {
        result = (dt != clk) ? 1 : -1;
    }
    last_clk = clk;
    return result;
}

// check when x rotray button is pressed
bool last_x_state = false; // global var to store the state

bool check_x_button() {
    bool current = gpio_get(X_SW);

    if (!last_x_state && current) {
        sleep_ms(100);
        last_x_state = current;
        return true;
    }
    last_x_state = current;
    return false;
}

// this is check when y button is pressed or not
bool last_y_state = false; 

bool check_y_button() {
    bool current = gpio_get(Y_SW);

    if (!last_y_state && current) {
        sleep_ms(100);
        last_y_state = current;
        return true;
    }
    last_y_state = current;
    return false;
}

// =========================================================== //

// int main() {
//     stdio_init_all();

//     // Initializing BUTTON_MODE
//     gpio_init(BUTTON_MODE);
//     gpio_set_dir(BUTTON_MODE, false);

//     // Initializing BUTTON_CAPTURE
//     gpio_init(BUTTON_CAPTURE);
//     gpio_set_dir(BUTTON_CAPTURE, false); 

//     // Initializing X encoder
//     gpio_init(X_CLK); 
//     gpio_set_dir(X_CLK, GPIO_IN);
//     gpio_pull_up(X_CLK);

//     gpio_init(X_DT);  
//     gpio_set_dir(X_DT, GPIO_IN);
//     gpio_pull_up(X_DT);

//     gpio_init(X_SW);  
//     gpio_set_dir(X_SW, GPIO_IN);
//     gpio_pull_up(X_SW);

//     // Initializing Y encoder
//     gpio_init(Y_CLK); 
//     gpio_set_dir(Y_CLK, GPIO_IN);
//     gpio_pull_up(Y_CLK);

//     gpio_init(Y_DT);  
//     gpio_set_dir(Y_DT, GPIO_IN);
//     gpio_pull_up(Y_DT);

//     gpio_init(Y_SW);  
//     gpio_set_dir(Y_SW, GPIO_IN);
//     gpio_pull_up(Y_SW);

//     printf("Wave Generator Ready\n");

//     wave_type current_wave = WAVE_OFF;
//     wave_output(current_wave);
    
//     // Initializing scaling steps and initial scales
//     float x_scale = 1.0f;
//     float y_scale = 1.0f;
//     float x_offset = 0.0f;
//     float y_offset = 0.0f;

//     float x_step = 0.3f;
//     float y_step = 0.3f;

//     bool x_mode = false;   // false for scale, true for shift
//     bool y_mode = false;   // same as above
//     // loop functions

//     while (1) {
//         // For the push buttons
//         if (check_mode_button()) {
//         // if (gpio_get(BUTTON_MODE)) {
//             current_wave = (current_wave + 1) % WAVE_COUNT;
//             wave_output(current_wave);
//         }

//         if (check_capture_button()) {
//             printf("Successfully Captured Waveform! :D\n");
//         }

//         // For the rotary encoder
//         // to check mode of the buttons
//         if (check_x_button()) {
//             x_mode = !x_mode;
//             int x_check_mode = x_mode ? "Shift" : "Scale";
//             printf("X mode: %s\n", x_check_mode);
//         }

//         if (check_y_button()) {
//             y_mode = !y_mode;
//             int y_check_mode = y_mode ? "Shift" : "Scale";
//             printf("Y mode: %s\n", y_check_mode);
//         }

//         // check what mode it is and scale or shift accordingly
//         int x_check_turn = read_encoder_x();
//         if (x_check_turn != 0) {
//             if (x_mode) {
//                 if (x_check_turn > 0) {
//                     x_offset += x_step;
//                 } else {
//                     x_offset -= x_step;
//                 }
//                 printf("X offset = %.2f\n", x_offset);

//             } else {
//                 if (x_check_turn > 0) {
//                     x_scale += x_step;
//                 } else {
//                     x_scale -= x_step;
//                     if (x_scale < 0.1f) x_scale = 0.1f;
//                 }
//                 printf("X scale = %.2f\n", x_scale);
//             }
//         }

//         int y_check_turn = read_encoder_y();
//         if (y_check_turn != 0) {
//             if (y_mode) {
//                 if (y_check_turn > 0) {
//                     y_offset += y_step;
//                 } else {
//                     y_offset -= y_step;
//                 }
//                 printf("Y offset = %.2f\n", y_offset);
                
//             } else {
//                 if (y_check_turn > 0) {
//                     y_scale += y_step;
//                 } else {
//                     y_scale -= y_step;
//                     if (y_scale < 0.1f) y_scale = 0.1f;
//                 }
//                 printf("Y scale = %.2f\n", y_scale);
//             }
//         }

//         sleep_ms(10);
//     }
// }