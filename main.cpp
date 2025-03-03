#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <cmath>
#include <iostream>
#include <vector>

#include "tty.h"
using namespace std;

#define CHANNELS 5
#define BUFFER_SIZE 4

#define M_PI (3.14159265)
#define TABLE_SIZE (100)

int main() {
  TTY tty;
  char port_name[32];
  sprintf_s(port_name, "\\\\.\\COM%d", 7);
  tty.Connect(port_name, 9600);

  float sine[TABLE_SIZE]{};
  int sample_phase = 0;
  for (int i = 0; i < TABLE_SIZE; i++) {
    sine[i] = (float)sin(((double)i / (double)TABLE_SIZE) * M_PI * 2.);
  }

  while (1) {
    float out_data[CHANNELS]{};
    for (int i = 0; i < CHANNELS; ++i) {
      //out_data[i] = sine[sample_phase];
      out_data[i] = 1;
    }
    sample_phase += 1;
    if (sample_phase >= TABLE_SIZE) sample_phase -= TABLE_SIZE;
    tty.Write(out_data);
  }

  return 0;
}