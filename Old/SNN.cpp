#include "../include/SNN.h"
using namespace std;



int readTopology(char *file, SNN *snn) {

	Layer currentLayer;
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
				if (!currentLayer.type.empty()) {
					snn->layers.push_back(currentLayer);
				}
				stream >> currentLayer.type;
			} else if (token.compare("neurons") == 0) {
				stream >> currentLayer.neurons;
			} else if (token.compare("connections") == 0) {
				stream >> currentLayer.connections;
			} else if (token.compare("sparse_connection") == 0 && currentLayer.connections == "sparse") {
				int from, to;
				stream >> from >> to;
				currentLayer.sparse_connections.push_back(make_pair(from, to));
			} 
		}
		if (!currentLayer.type.empty()) {
			snn->layers.push_back(currentLayer);
		}
		network_file.close();
	}
	return 0;
};

void viewTopology(SNN *snn) {
	cout << "-- NETWORK TOPOLOGY --" << endl;
	// cout << snn->layers.size() << " layers" << endl;
	for (int i = 0; i < snn->layers.size(); i++) {
		cout << "LAYER " << i << ": " << snn->layers[i].type << endl;
		cout << "Neurons: " << snn->layers[i].neurons << endl;
		cout << "Connections: " << snn->layers[i].connections << endl;
		if (snn->layers[i].connections == "sparse") {
			cout << "Sparse connections: " << endl;
			for (int j = 0; j < snn->layers[i].sparse_connections.size(); j++) {
				cout << snn->layers[i].sparse_connections[j].first << " -> " << snn->layers[i].sparse_connections[j].second << endl;
			}
		}
		cout << endl;
	}
	
}