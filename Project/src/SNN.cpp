#include "../include/SNN.h"
#include "../include/Globals.h"
using namespace std;

SNN::SNN() {
	
}

int SNN::initNetwork(char &file) {

	string type;
    int numNeurons;
    string connections;
	int multisynaptic;
    vector<pair<int, int>> sparseConnections;
    // vector<LIFneuron> neurons;

	string line;

	// Read network file
	ifstream network_file(&file);
	if (!network_file.is_open()) {
		cout << "Error: Unable to open file " << &file << endl;
		return 1;
	} else {
		cout << "Reading file " << &file << endl;
		while (getline(network_file, line)) {
			if (line.length() == 0 || line[0] == '#') continue;

			istringstream stream(line);
    		string token;
			
			stream >> token;
			if (token.compare("layer") == 0) {
				if (!type.empty()) {
					Layer currentLayer(type, numNeurons, connections, multisynaptic, sparseConnections);
					layers.push_back(currentLayer);
				}
				stream >> type;
			} else if (token.compare("neurons") == 0) {
				stream >> numNeurons;
			} else if (token.compare("connections") == 0) {
				stream >> connections;
			} else if (token.compare("multisynaptic") == 0) {
				stream >> multisynaptic;
			} else if (token.compare("sparse_connection") == 0 && connections == "sparse") {
				int from, to;
				stream >> from >> to;
				sparseConnections.push_back(make_pair(from, to));
			} 
		}
		if (!type.empty()) {
			Layer currentLayer(type, numNeurons, connections, multisynaptic, sparseConnections);
			layers.push_back(currentLayer);
		}
		network_file.close();
	}

	linkLayers();

	return 0;
};

void SNN::linkLayers() {
	for (int i = 0; i < layers.size() - 1; i++) {
		layers[i].setPostSynapticLinks(layers[i + 1]);
		layers[i].initWeights(layers[i + 1].getNumNeurons());
		layers[i].initDelays(layers[i + 1].getNumNeurons(), MIN_DELAY, MAX_DELAY);
		layers[i].initPreSynapticTrace(layers[i + 1].getNumNeurons());
	}
}

void SNN::viewTopology() {
	cout << "-- NETWORK TOPOLOGY --" << endl;
	for (int i = 0; i < layers.size(); i++) {
		cout << "LAYER " << i << ": " << layers[i].getType() << endl;
		cout << "Neurons: " << layers[i].getNumNeurons() << endl;
		cout << "Connections: " << layers[i].getConnections() << endl;
		cout << "Multisynaptic: " << layers[i].getMultisynaptic() << endl;
		if (layers[i].getConnections() == "sparse") {
			cout << "Sparse connections: " << endl;
			for (int j = 0; j < layers[i].getSparseConnections().size(); j++) {
				cout << layers[i].getSparseConnections()[j].first << " -> " << layers[i].getSparseConnections()[j].second << endl;
			}
		}
		cout << endl;
	}
}

void SNN::trainNetwork(double t, double dt) {
	cout << "-- TRAINING NETWORK --" << endl;
	for (double ct = 0.0; ct < t; ct+=dt) {
		for (int i = 0; i < layers.size() - 1; i++) {	
			cout << "- Layer " << i << " -" << endl;
			layers[i + 1].updatePreSynapticTrace(layers[i], ct, 0.1);// ALPHA);
			layers[i].propagateSpikes(layers[i + 1], ct);
		}
	}
}
