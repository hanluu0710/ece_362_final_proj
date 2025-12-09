// all wavegen code here
#if WAVEGEN

#include "wavegen.h"
#include "hardware/pwm.h"
#include "support.h"
#include <math.h>

WaveGenConfig wavegen_config = {
.type = WAVE_OFF,
.frequency = 1000.0f, //default frequency : 1 khz
.amplitude = 1.0f, // default amp : 1v
.enabled = false // Initially disabled
};


static uint wavegen_pin = 0; // GPIO pin used for pwm output
static uint wavegen_slice = 0; // pwm slice index associated with the pin
static uint wavegen_chan = 0; // pwm channel (A/B) for that pin


#define WAVEGEN_MAX_VOLT 3.3f // max reference voltage
#define MID_VALUE 16384.0f // midpoint sampel value
#define BASE_AMP 16383.0f // base amplitude used for normalized


//clamp a float value x into the range (low, high)
static inline float clampf(float x, float lo, float hi){
    if (x < lo) 
    return lo;

    if (x > hi) 
    return hi;

    return x;
}

// (from David) This is needed because the oscilloscope can't read the PWM, it's a hardware thingy. 
// It's not checking for the voltage sample
float wavegen_get_sample(float t)
{
    if (!wavegen_config.enabled)
        return 0.0f;

    float amplitude = wavegen_config.amplitude;
    float freq = wavegen_config.frequency;

    float phase = freq * t;

    switch (wavegen_config.type)
    {
        case WAVE_SINE:
            return amplitude * sinf(2.0f * M_PI * phase);

        case WAVE_SQUARE:
            return (sinf(2.0f * M_PI * phase) > 0 ? amplitude : -amplitude);

        case WAVE_SAWTOOTH:
            return amplitude * (2.0f * (phase - floorf(phase + 0.5f)));

        case WAVE_TRIANGLE:
            return amplitude * (2.0f * fabsf(2.0f * (phase - floorf(phase + 0.5f))) - 1.0f);

        default:
            return 0.0f;
    }
}


static void wavegen_fill_wavetable(WaveType, float amp_volt)
{
    float norm_amp = clampf(amp_volt/WAVEGEN_MAX_VOLT, 0, 1);
    float amp = BASE_AMP * norm_amp;

    // if waveform is off, or amp is zero/non-positive, output a flat 0 signal
    if (type == WAVE_OFF || amp_volt <= 0){
        for (int i = 0; i < N; ++i) {
            wavetable[i] = 0; // zero out all samples
        }
        return; 
    }

    //generate one period of the waveform with N samples
    for (int i = 0; i < N; ++i) {// map index i (0~N-1) to a phase in [0, 2pi]
        float t = (float) i/N;
        float phase = 2.0f * (float)M_PI * (float)i / (float)N; // normalized waveform sample in -1.0~ +1.0
        float s = 0.0f;


        // chose waveform shape
        switch (type) {
            case WAVE_SINE: // sine wave: -1 to 1
            s = sinf(2*M_PI*t);
            break;

            case WAVE_SAWTOOTH: {//sawtooth wave using a linear ramp from 0 to 1 mapped to -1 to 1 
                float t = (float)i / (float)N;
                s = 2 * t - 1;
                break;
            }
            case WAVE_TRIANGLE: { //triangle wave built from piecewise linear segments
                float t = (float)i / (float)N;
                if (t < 0.25f) //first quarter: ramp up from 0 to ~1
                s = 4.0f * t;
                else if (t < 0.74f)
                s = 2.0f - 4.0f * t; // middle seg
                else
                s = 4 * t - 4; // final seg : ramp from negative region up toward
                break;
            }
            case WAVE_SQUARE: // square wave: +1 for first half, -1 for second half
            s = (i < N/2) ? 1 : -1;
            break;
            default: // unknow type: just ouput 0
                s = 0;
        }

        // Convert normalized sample s (-1~1) to table value around mid_value
        float v = MID_VALUE + amp * s;
        if (v < 0){
            v = 0.0f;
        }
        if (v > 32767){
            v = 32767;
        }
        wavetable[i] = (short int)v; // store as short int in wavetable
    }
}

// PWM interrupt handler
// Called whenever the PWM counter wraps, to update the output sample
static void wavegen_pwm_irq()
{
    pwm_clear_irq(wavegen_slice); //clear the interrupt flag for this pwm slice

    offset0 += step0; //advance the fixed point phase accumulator
    if (offset0 >= (N << 16)){
        offset0 -= ( N << 16);
    }

    // Use upper bits of offset0 as the wavetable index (Q16 -> integer index)
    int samp = wavetable[offset0 >> 16];

    // Get the current PWM period (= top + 1)
    int period = (int)pwm_hw ->slice[wavegen_slice].top + 1;

    // Scale the 16-bit sample to the PWM resolution
    int level = (samp * period) >> 16;

    if (level < 0) 
    level = 0;
    if (level > period)
    level = period;

    pwm_set_chan_level(wavegen_slice, wavegen_chan, level); //update pwm duty cycle for this channel

}


// Initialize the wave generator on a specific GPIO pin
// pwm_pin: GPIO number to be used as PWM output
void wavegen_init(uint pwm_pin)
{
    wavegen_pin = pwm_pin; // save the selected pin

    gpio_set_function(wavegen_pin, GPIO_FUNC_PWM); //config the pin aa pwm output
    wavegen_slice = pwm_gpio_to_slice_num(wavegen_pin);
    wavegen_chan = pwm_gpio_to_channel(wavegen_pin);

    pwm_set_clkdiv(wavegen_slice, 150.0f); //set pwm clock divider

    int period = (1000000 / RATE) - 1;
    pwm_set_wrap(wavegen_slice, period);
    pwm_set_chan_level(wavegen_slice, wavegen_chan, 0);// start with 0 duty cycle

    wavegen_fill_wavetable(WAVE_SINE, wavegen_config.amplitude);
    set_freq(0, wavegen_config.frequency);

    pwm_set_irq0_enabled(wavegen_slice, true); //enable pwm interrupt for this slice
    irq_set_exclusive_handler(PWM_IRQ_WRAP, wavegen_pwm_irq);
    irq_set_enabled(PWM_IRQ_WRAP, true); // Enable the PWM interrupt at the NVIC level


    pwm_set_enabled(wavegen_slice, true);
}

// Change the waveform type (OFF, SINE, SAWTOOTH, TRIANGLE, SQUARE)
void wavegen_set_type(WaveType type)
{
    if (type < 0 || type >= WAVE_COUNT) return; //bound check
    wavegen_config.type = type; //update global config

    wavegen_fill_wavetable(wavegen_config.type, wavegen_config.amplitude);//rebuild wavetable using the new type and current amp
}

void wavegen_set_frequency(float freq_hz)//change the waveform freq
{
    if (freq_hz < 0.0f) freq_hz = 0.0f;
    wavegen_config.frequency = freq_hz;

    if (wavegen_config.enabled && wavegen_config.frequency > 0.0f) {// If the generator is enabled and frequency is positive, apply it

        set_freq(0, wavegen_config.frequency);// update step0 to match the new freq
    } else { //when disabled or zero freq, stop phase prograssion
        set_freq(0, 0.0f); // stop
    }
}

void wavegen_set_amplitude(float amp_volt)// Change the waveform amplitude in volts
{
    wavegen_config.amplitude = clampf(amp_volt, 0.0f, WAVEGEN_MAX_VOLT);// Clamp amplitude to [0, 3.3] V and store it

    wavegen_fill_wavetable(wavegen_config.type, wavegen_config.amplitude);// Rebuild wavetable with the updated amplitude and current type
}


// Enable or disable the wave generator
void wavegen_enable(bool enable)
{
    wavegen_config.enabled = enable; //// Update the global enabled flag

    if (!enable) {// When disabling: stop the frequency and force duty cycle to 0
        set_freq(0, 0.0f);           
        pwm_set_chan_level(wavegen_slice, wavegen_chan, 0);
    } else { // When enabling: apply the current stored frequency
        set_freq(0, wavegen_config.frequency);
    }
}

// void wavegen_update(void)
// {
// }

#endif