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
	// snn.viewTopology();

	snn.trainNetwork(TIME, 0.1);

	// double dt = 0.1;
	// double simTime = 100.0;
	// double iSyn = 0.0;
	// double spike = 0.0;

	// LIFneuron neuron;
	
	// for (double t = 0; t < simTime; t += dt) {
	// 	spike = (t > 10 && t < 12) ? 1 : 0;
	// 	iSyn = (t > 10 && t < 12) ? 0.5 : 0;
		
	// 	neuron.updateTrace(spike);
	// 	neuron.updateMembranePotential(iSyn, t);
	// 	// std::cout << "Time: " << std::fixed << std::setprecision(1) << t << " ms, Post-neuron membrane potential: " << neuron.getMembranePotential() << " mV " << std::endl;
	// }

	return 0;
}