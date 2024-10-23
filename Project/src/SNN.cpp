#include "../include/SNN.h"
using namespace std;

SNN::SNN() {
	
}

int SNN::initNetwork(char *file) {

	string type;
    int numNeurons;
    string connections;
    vector<pair<int, int>> sparseConnections;
    // vector<LIFneuron> neurons;

	string line;

	// Read network file
	ifstream network_file(file);
	if (!network_file.is_open()) {
		cout << "Error: Unable to open file " << file << endl;
		return 1;
	} else {
		cout << "Reading file " << file << endl;
		while (getline(network_file, line)) {
			if (line.length() == 0 || line[0] == '#') continue;

			istringstream stream(line);
    		string token;
			
			stream >> token;
			if (token.compare("layer") == 0) {
				if (!type.empty()) {
					Layer currentLayer;
					currentLayer.initLayer(type, numNeurons, connections, sparseConnections);
					layers.push_back(currentLayer);
				}
				stream >> type;
			} else if (token.compare("neurons") == 0) {
				stream >> numNeurons;
				cout << "Number of neurons: " << numNeurons << endl;
			} else if (token.compare("connections") == 0) {
				stream >> connections;
			} else if (token.compare("sparse_connection") == 0 && connections == "sparse") {
				int from, to;
				stream >> from >> to;
				sparseConnections.push_back(make_pair(from, to));
			} 
		}
		if (!type.empty()) {
			Layer currentLayer;
			currentLayer.initLayer(type, numNeurons, connections, sparseConnections);
			layers.push_back(currentLayer);
		}
		network_file.close();
	}
	return 0;
};

void SNN::viewTopology() {
	cout << "-- NETWORK TOPOLOGY --" << endl;
	for (int i = 0; i < layers.size(); i++) {
		cout << "LAYER " << i << ": " << layers[i].getType() << endl;
		cout << "Neurons: " << layers[i].getNumNeurons() << endl;
		cout << "Connections: " << layers[i].getConnections() << endl;
		if (layers[i].getConnections() == "sparse") {
			cout << "Sparse connections: " << endl;
			for (int j = 0; j < layers[i].getSparseConnections().size(); j++) {
				cout << layers[i].getSparseConnections()[j].first << " -> " << layers[i].getSparseConnections()[j].second << endl;
			}
		}
		cout << endl;
	}
	
}