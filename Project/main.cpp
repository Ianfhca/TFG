#include "neuron.h"

int main() {
  bool stop = true;

  LIF_neuron neuron(-75, -55, 10, 10, -75, -75, 2);
  neuron.update_membrane_potential(stop);

  return 0;
}