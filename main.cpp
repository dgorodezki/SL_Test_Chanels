#define _USE_MATH_DEFINES
#include <cmath>
#include "tty.h"

#define PORT 7
#define CHANNELS 5

#define TABLE_SIZE 1024

int main() {
  TTY tty;
  char port_name[32];
  sprintf_s(port_name, "\\\\.\\COM%d", PORT);
  tty.Connect(port_name, 128000);

  float sine[TABLE_SIZE]{};
  for (int i = 0; i < TABLE_SIZE; i++) {
    sine[i] = (float)sin(((double)i / (double)TABLE_SIZE) * M_PI * 2.0);
  }

  float out_data[CHANNELS]{};
  int sample_phase = 0;
  while (tty.IsOK()) {
    for (int i = 0; i < CHANNELS; ++i) {
      out_data[i] = sine[sample_phase];
    }
    sample_phase += 1;
    if (sample_phase >= TABLE_SIZE) sample_phase -= TABLE_SIZE;

    tty.Write(out_data, CHANNELS);
  }

  return 0;
}