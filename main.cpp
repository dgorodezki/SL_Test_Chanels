#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "tty.h"

#define PORT 7
#define CHANNELS 5

#define TABLE_SIZE 32

/**
 * Генерирует синусоидальный сигнал с заданной частотой и сэмплрейтом.
 *
 * @param buffer - указатель на буфер, в который будут записаны сэмплы
 * @param num_samples - количество сэмплов, которое нужно заполнить
 * @param sample_rate - частота дискретизации (Гц)
 * @param frequency - частота синусоиды (Гц)
 */
static void generate_sine_wave(int16_t* buffer_16, float* buffer_32,
                               size_t num_samples, int sample_rate,
                               double frequency) {
    for (size_t i = 0; i < num_samples; i++) {
        double t = (double)i / sample_rate;  // Время в секундах
        double value = sin(2.0 * M_PI * frequency * t);
        buffer_16[i] = (int16_t)(value * 32767);
        buffer_32[i] = (float)value;
    }
}

int main() {
    TTY tty;
    char port_name[32];
    sprintf_s(port_name, "\\\\.\\COM%d", PORT);
    tty.Connect(port_name, 128000);

    int sample_rate = 44100;
    int frequency = 10;
    int num_samples = sample_rate / frequency;
    int16_t* sine_16 = new int16_t[num_samples];
    float* sine_32 = new float[num_samples];
    generate_sine_wave(sine_16, sine_32, num_samples, sample_rate, frequency);

    float out_data[CHANNELS]{};
    int sample_phase = 0;
    while (tty.IsOK()) {
        for (int i = 0; i < CHANNELS; ++i) {
            out_data[i] = sine_32[sample_phase];
        }
        sample_phase += 1;
        if (sample_phase >= num_samples) sample_phase -= num_samples;

        tty.Write(out_data, CHANNELS);
    }

    delete[] sine_16;
    delete[] sine_32;
    return 0;
}