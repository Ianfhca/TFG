#include "neuron.h"


int main() {
  bool stop = true;

  LIF_neuron neuron(-55, -75, 10, 10, -75, -75, 2);
  neuron.update_membrane_potential(stop);
  neuron.plot_membrane_potential();

  return 0;
}