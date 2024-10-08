#include "../include/LIFneuron.h"
#include "../include/SNN.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return 1;
	}

	SNN snn;

	readTopology(argv[1], &snn);

	viewTopology(&snn);

	bool stop = true;

	LIFneuron neuron(-55, -75, 10, 10, -75, -75, 2);
	// neuron.updateMembranePotential(stop);

	return 0;
}