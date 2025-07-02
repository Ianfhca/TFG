#include "../include/Globals.hpp"
#include "../include/SNN.hpp"
#include "../include/Utils.hpp"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;
using namespace chrono;

int main(int argc, char *argv[]) {

	if (argc < 3 || argc > 4) {
		cerr << "Usage: " << argv[0] << " -train | -test" << " <topology file, with no .ext>" << endl;
		return 1;
	}

	SNN snn;
	string mode = "train";
	auto start = high_resolution_clock::now();
	initializeGenerator();

	if (string(argv[1]) != "-train" && string(argv[1]) != "-test") {
		cerr << "Error: Invalid option. Use -train or -test." << endl;
		return 1;
	} else if (string(argv[1]) == "-train") {
		mode = "train";
		if (snn.initNetwork(*argv[2], mode) != 0) {
			cerr << "Error: Unable to initialize network." << endl;
			return 1;
		} else {
			snn.linkLayers();
			snn.viewTopology();
			snn.trainNetwork();
			snn.saveWeights();
		}
	} else if (string(argv[1]) == "-test") {
		mode = "test";
		if (snn.initNetwork(*argv[2], mode) != 0) { 
			cerr << "Error: Unable to initialize network." << endl;
			return 1;
		} else {
			snn.linkLayers();
			snn.viewTopology();
			if (snn.loadWeights() != 0) {
				cerr << "Error: Unable to load weights." << endl;
				return 1;
			} else {
				snn.testNetwork();
			}
		}
	}

	setColor("blue"); cout << "\n-- END OF PROGRAM --\n" << endl; setColor("reset");

	auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    if ((duration.count() / 1000 / 60) < 1) cout << "Duration: " << (duration.count() / 1000) << " sec" << endl;
    else cout << "Duration: " << (duration.count() / 1000 / 60) << " min" << endl;

	return 0;
}