#ifndef WAVEGEN_H
#define WAVEGEN_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
//#include "PrintCore/stdlib.h"

typedef enum {
    WAVE_OFF = 0,
    WAVE_SINE,
    WAVE_SAWTOOTH,
    WAVE_TRIANGLE,
    WAVE_SQUARE,
    WAVE_COUNT
} WaveType;
// Wavefrom generator configuration structure
typedef struct {
    WaveType type;      //Current waveform type
    float frequency;    //Frequency in Hz
    float amplitude;    //Amplitude (0v to 3.3v)
    bool enabled;       //Enable/Disable
} WaveGenConfig;

extern WaveGenConfig wavegen_config;

void wavegen_init(uint32_t pin);
void wavegen_set_type(WaveType type);
void wavegen_set_frequency(float freq);
void wavegen_set_amplitude(float amp);
void wavegen_enable(bool enable);
void wavegen_update(void);
float wavegen_get_sample(float t);


#endif 
