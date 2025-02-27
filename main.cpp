#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <cmath>
#include <iostream>
#include <vector>

#include "tty.h"
using namespace std;
#define SAMPLE_RATE 48000
#define CHANNELS 4
#define BUFFER_SIZE 8

int main() {
  TTY tty;
  char port_name[32];
  sprintf_s(port_name, "\\\\.\\COM%d", 7);

  tty.Connect(port_name, 9600);

  std::vector<float> data(BUFFER_SIZE * CHANNELS);

  for (int i = 0; i < BUFFER_SIZE * CHANNELS; i += 4) {
    float sample =
        (float)sin(((double)i / (double)BUFFER_SIZE * CHANNELS) * 3.14 * 2.);
    //sample = 1;
    data[i] = sample;
    data[i + 1] = sample;
    data[i + 2] = sample;
    data[i + 3] = sample;
  }

  for (int i = 0; i < BUFFER_SIZE * CHANNELS; ) {
    float out_data[4] = {data[i++], data[i++], data[i++], data[i++]};
    tty.Write(out_data);
  }

  return 0;
}