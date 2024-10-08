#include "../include/SNN.h"
using namespace std;

int readTopology(char *file, SNN *snn) {

	string line;


	Layer currentLayer;

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
			// cout << "Token: " << token << endl;
			if (token.compare("layer") == 0) {
				// cout << "Hola";
				if (!currentLayer.type.empty()) {
					snn->layers.push_back(currentLayer);
				}
				stream >> currentLayer.type;
				// currentLayer.type = token2;
				// cout << "Layer: " << currentLayer.type << endl;
			} else if (token.compare("neurons") == 0) {
				stream >> currentLayer.neurons;
				// cout << "Neurons: " << currentLayer.neurons << endl;
			} else if (token.compare("connections") == 0) {
				stream >> currentLayer.connections;
				// cout << "Connections: " << currentLayer.connections << endl;
			} else if (token.compare("custom_connection") == 0 && currentLayer.connections == "custom") {
				int from, to;
				stream >> from >> to;
				currentLayer.custom_connections.push_back(make_pair(from, to));
			}
			//cout << "Token: " << token << endl;
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
	cout << snn->layers.size() << " layers" << endl;
	for (int i = 0; i < snn->layers.size(); i++) {
		cout << "Layer " << i << ": " << snn->layers[i].type << endl;
		cout << "Neurons: " << snn->layers[i].neurons << endl;
		cout << "Connections: " << snn->layers[i].connections << endl;
		if (snn->layers[i].connections == "custom") {
			cout << "Custom connections: " << endl;
			for (int j = 0; j < snn->layers[i].custom_connections.size(); j++) {
				cout << snn->layers[i].custom_connections[j].first << " -> " << snn->layers[i].custom_connections[j].second << endl;
			}
		}
	}
}