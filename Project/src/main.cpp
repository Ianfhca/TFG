#include "../include/LIFneuron.h"
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <input file>" << endl;
    return 1;
  }

  string file = argv[1];

  // Read network file
  ifstream network_file(file);
  if (!network_file.is_open()) {
    cout << "Error: Unable to open file " << file << endl;
    return 1;
  } else {
    cout << "Reading file " << file << endl;

    network_file.close();
  }
  

  bool stop = true;

  LIFneuron neuron(-55, -75, 10, 10, -75, -75, 2);
  // neuron.updateMembranePotential(stop);

  return 0;
}