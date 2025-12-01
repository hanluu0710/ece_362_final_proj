#include "pico/stdlib.h"
#include <stdio.h>
#include <stdbool.h>
#include "buttons.h"

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

void init_gpio()
{
    // Initializing BUTTON_MODE
    gpio_init(BUTTON_MODE);
    gpio_set_dir(BUTTON_MODE, false);
    gpio_pull_down(BUTTON_MODE); // 1 = pressed, 0 = released

    // Initializing BUTTON_CAPTURE
    gpio_init(BUTTON_CAPTURE);
    gpio_set_dir(BUTTON_CAPTURE, false);
    gpio_pull_down(BUTTON_CAPTURE); // IMPORTANT

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

// Encoder Counts (from interrupt)
volatile int x_encoder_steps = 0;
volatile int y_encoder_steps = 0;

// Encoder Modes
int x_mode = 0;  // 0 = scale, 1 = shift
int y_mode = 0;
bool x_mode_changed = false;
bool y_mode_changed = false;


// Shift and Scale Variables
float x_offset = 0;
float y_offset = 0;

extern int volts_per_div;
extern int time_per_div;
extern float dt;
extern bool grid_dirty;

//Capture
extern bool pause;
wave_type current_wave = WAVE_OFF;
void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == X_CLK){
        int clk = gpio_get(X_CLK);
        int dt  = gpio_get(X_DT);
        x_encoder_steps += (dt != clk) ? 1 : -1;
        x_encoder_steps %= 10;
    }
    else if (gpio == Y_CLK){
        int clk = gpio_get(Y_CLK);
        int dt  = gpio_get(Y_DT);
        y_encoder_steps += (dt != clk) ? 1 : -1;
    }
    else if (gpio == BUTTON_CAPTURE) {
        pause = !pause;
        printf("PAUSE = %d\n", pause);
    }
    else if (gpio == BUTTON_MODE){
        current_wave = (current_wave + 1) % WAVE_COUNT;
        wave_output(current_wave);
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
    &gpio_callback
    );

    gpio_set_irq_enabled(
        Y_CLK,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true
    );
    gpio_set_irq_enabled(
        BUTTON_CAPTURE,
        GPIO_IRQ_EDGE_RISE,
        true
    );
    gpio_set_irq_enabled(
        BUTTON_MODE,
        GPIO_IRQ_EDGE_RISE,
        true
    );

}
bool last_x_button = false;
bool last_y_button = false;
bool last_mode_button = false;
bool last_capture_button = false;

bool check_capture_button(){
    int current_state = gpio_get(BUTTON_CAPTURE);
    if (!last_capture_button && current_state)
    {
        last_capture_button = current_state;
        return true;
    }
    last_capture_button = current_state;
    return false;
}
bool check_mode_button() {
    static bool last_state = false;
    bool current_state = gpio_get(BUTTON_MODE);

    if (!last_state && current_state) {
        last_state = current_state;
        return true;
    }
    last_state = current_state;
    return false;
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


void controls()
{
    // Handle X Button (toggle mode)
    if (check_button(X_SW, &last_x_button))
    {
        x_mode = !x_mode;
        x_mode_changed = true;
        printf("X Mode: %s\n", x_mode ? "Shift" : "Scale");
    }

    // Handle Y Button (toggle mode)
    if (check_button(Y_SW, &last_y_button))
    {
        y_mode = !y_mode;
        y_mode_changed = true;
        printf("Y Mode: %s\n", y_mode ? "Shift" : "Scale");
    }

    // MODE button (wave change)

    // CAPTURE button

    // X encoder steps
    int steps_x = x_encoder_steps;
    x_encoder_steps = 0;
    int factor = 1;
    if (steps_x != 0)
    {
        if (x_mode == 1) // SHIFT
        {
            x_offset += steps_x * 0.3f;
            printf("X Offset = %.2f\n", x_offset); 
        }
        else // SCALE (time)
        {
            if (time_per_div>=1000000){ //if > 1s
                factor = 1000000; // +1s
            }
            else if (time_per_div >= 100000 ){ //if > 100ms
                factor = 100000; // +100ms
            }
            else if (time_per_div >= 10000 ){ //if > 10ms
                factor = 10000; // +10ms
            }
            else if (time_per_div >= 1000 ){ //if > 1ms
                factor = 1000; // +1ms
            }
            else if (time_per_div >= 100){ //if > 100us
                factor = 100; // +100us
            }
            else if (time_per_div >= 10){
                factor = 10; // +10us
            }
            else {
                factor = 1;
            }

            time_per_div += factor*steps_x;
            if (time_per_div<1){
                time_per_div =1;
            }
            time_per_div %= 10000001; //min = 1us; max = 10s; +5ms every increment
            //printf("Time/div = %dus\n", time_per_div);         
            if (time_per_div>1000000){ //if > 1s
                printf("Time/div = %ds\n", time_per_div/1000000);
            }
            else if (time_per_div > 1000){ //if > 1ms
                printf("Time/div = %dms\n", time_per_div/1000);
            }
            else {
                printf("Time/div = %dus\n", time_per_div);
            }
        }
        grid_dirty = true;
    }

    // Y encoder steps
    int steps_y = y_encoder_steps;
    y_encoder_steps = 0;

    if (steps_y != 0)
    {
        grid_dirty = true;
        if (y_mode == 1) // SHIFT
        {
            y_offset = y_offset + steps_y * volts_per_div;
            printf("Y Offset = %.2f\n", y_offset);
        }
        else // SCALE (voltage)
        {
            volts_per_div += steps_y;
            volts_per_div %= 11;
            if (volts_per_div < 1) volts_per_div = 1;
            printf("Volts/div = %dV\n", volts_per_div);
        }
    }
}
