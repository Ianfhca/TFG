#include "../include/Globals.hpp"
#include "../include/SNN.hpp"
#include "../include/Utils.hpp"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;
using namespace chrono;

int main(int argc, char *argv[]) { 

	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <input file>" << endl;
		return 1;
	}

	auto start = high_resolution_clock::now();
	SNN snn;
	if (snn.initNetwork(*argv[1]) != 0) {
		cerr << "Error: Unable to initialize network." << endl;
		return 1;
	}

	snn.linkLayers();
	snn.viewTopology();
	snn.viewInputSpikes();

	snn.trainNetwork();

	setColor("blue"); cout << "-- END OF PROGRAM --" << endl; setColor("reset");

	auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Duration: " << duration.count() << " ms\n";
    
	return 0; 
} 