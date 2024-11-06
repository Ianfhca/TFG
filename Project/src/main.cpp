#include "../include/Globals.h"
#include "../include/LIFneuron.h"
#include "../include/SNN.h"

#include <iostream>
#include <iomanip> 

using namespace std;

int main(int argc, char *argv[]) {

	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <input file>" << endl;
		return 1;
	}

	SNN snn;
	snn.initNetwork(*argv[1]);
	// snn.linkLayers();
	snn.viewTopology();

	snn.trainNetwork(TIME, DT);

	return 0;
}