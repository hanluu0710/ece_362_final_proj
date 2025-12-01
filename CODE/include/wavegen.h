#ifndef WAVEGEN_H
#define WAVEGEN_H

#include<stdint.h>
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
    WaveType;           //Current waveform type
    float frequency;    //Frequency in Hz
    float amplitude;    //Amplitude (0v to 3.3v)
    bool enabled;       //Enable/Disable
} WaveGenConfig;

extern WaveGenCongif wavegen_congfig;

void wavegen_init(pin);
void wavegen_set_type(WaveType type);
void wavegen_set_frequency(float freq);
void wavegen_set_amplitude(float amp);
void wavegen_enable(bool enable);
void wavegen_update(void);


#endif 
