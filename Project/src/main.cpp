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
		cerr << "Usage: " << argv[0] << " -train" << " <topology file>" << endl;
		cerr << "Usage: " << argv[0] << " -test" << " <topology file>" << " <weight file number>" << endl;
		return 1;
	}

	SNN snn;
	int numFile = -1;
	auto start = high_resolution_clock::now();

	if (string(argv[1]) != "-train" && string(argv[1]) != "-test") {
		cerr << "Error: Invalid option. Use -train or -test." << endl;
		return 1;
	} else if (string(argv[1]) == "-train") {
		if (!filesystem::exists(argv[2])) {
			cerr << "Error: Input file does not exist." << endl;
			return 1;
		}
		if (filesystem::is_directory(argv[2])) {
			cerr << "Error: Input file is a directory, not a file." << endl;
			return 1;
		}
		if (snn.initNetwork(*argv[2]) != 0) { 
			cerr << "Error: Unable to initialize network." << endl;
			return 1;
		} else {
			snn.linkLayers();
			snn.viewTopology();
			snn.trainNetwork();
			snn.saveWeights();
		}
	} else if (string(argv[1]) == "-test") {
		if (!filesystem::exists(argv[2])) {
			cerr << "Error: Input file does not exist." << endl;
			return 1;
		}
		if (filesystem::is_directory(argv[2])) {
			cerr << "Error: Input file is a directory, not a file." << endl;
			return 1;
		}
		if (!isdigit(argv[3][0])) {
			cerr << "Error: Invalid weight file number." << endl;
			return 1;
		} else {
			numFile = stoi(argv[3]);
		}
		if (snn.initNetwork(*argv[2]) != 0) { 
			cerr << "Error: Unable to initialize network." << endl;
			return 1;
		} else {
			snn.linkLayers();
			snn.viewTopology();
			if (snn.loadWeights(numFile) != 0) {
				cerr << "Error: Unable to load weights." << endl;
				return 1;
			} else {
				snn.testNetwork();
				// snn.viewInputSpikes();
			}
		}
		
	}

	// auto start = high_resolution_clock::now();
	// SNN snn;
	// if (snn.initNetwork(*argv[2]) != 0) { 
	// 	cerr << "Error: Unable to initialize network." << endl;
	// 	return 1;
	// }

	// snn.linkLayers();
	// snn.viewTopology();

	// snn.trainNetwork();
	// snn.saveWeights();

	setColor("blue"); cout << "\n-- END OF PROGRAM --\n" << endl; setColor("reset");

	auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    if ((duration.count() / 1000 / 60) < 1) cout << "Duration: " << (duration.count() / 1000) << " sec" << endl;
    else cout << "Duration: " << (duration.count() / 1000 / 60) << " min" << endl;
	return 0;
}