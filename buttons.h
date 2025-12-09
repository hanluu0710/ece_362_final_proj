#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>

// Button pins
#define BUTTON_MODE 3
#define BUTTON_CAPTURE 4

#define X_CLK 5
#define X_DT 6
#define X_SW 7

#define Y_CLK 8
#define Y_DT 9
#define Y_SW 10

// Function declarations
void init_gpio(void);
void controls(void);

bool check_mode_button(void);
bool check_capture_button(void);
int read_encoder_x(void);
int read_encoder_y(void);
bool check_x_button(void);
bool check_y_button(void);

extern bool x_mode_changed;
extern bool y_mode_changed;
extern float x_offset;
extern float y_offset;
extern bool capture_mode;

typedef enum {
    WAVE_OFF,
    WAVE_SINE,
    WAVE_SAWTOOTH,
    WAVE_TRIANGLE,
    WAVE_SQUARE,
    WAVE_COUNT
} wave_type;


#endif