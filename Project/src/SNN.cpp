#include "../include/SNN.h"
#include "../include/Globals.h"
using namespace std;

SNN::SNN() {
	
}

int SNN::initNetwork(char &file) {

	string type = "Undefined";
    int numNeurons = -1;
    string connections = "Undefined";
	int multisynaptic = -1;
    vector<pair<int, int>> sparseConnections = {};

	double vReset = V_RESET;
	double vRest = V_REST;
	double v = V;
	double vTh = V_TH;
	double lambdaV = LAMBDA_V;
	double tRefr = T_REFR;
	double lambdaX = LAMBDA_X;
	double alpha = ALPHA;
	vector<pair<vector<double>, int>> neuronParams;

	string currentSection;
	string line;

	// Read network file
	ifstream network_file(&file);
	if (!network_file.is_open()) {
		cout << "Error: Unable to open file " << &file << endl;
		return -1;
	} else {
		cout << "Reading file " << &file << endl;
		while (getline(network_file, line)) {

			if (line.length() == 0 || line[0] == '#') continue;

			 if (line.rfind("--", 0) == 0) {
                currentSection = line.substr(3, line.size() - 6);
                cout << "Entering section: " << currentSection << endl;
                continue;
            }

			if (currentSection == "PARAMETERS") {
                istringstream stream(line);
				string token;
				
				stream >> token;
				if (token.compare("v_reset") == 0) {
					stream >> vReset;
				} else if (token.compare("v_rest") == 0) {
					stream >> vRest;
				} else if (token.compare("v") == 0) {
					stream >> v;
				} else if (token.compare("v_th") == 0) {
					stream >> vTh;
				} else if (token.compare("lambda_v") == 0) {
					stream >> lambdaV;
				} else if (token.compare("t_refr") == 0) {
					stream >> tRefr;
				} else if (token.compare("lambda_x") == 0) {
					stream >> lambdaX;
				} else if (token.compare("alpha") == 0) {
					stream >> alpha;
				} else {
					neuronParams.push_back(make_pair(vector<double>{vReset, vRest, v, vTh, lambdaV, tRefr, lambdaX, alpha}, 0));

					vReset = V_RESET;
					vRest = V_REST;
					v = V;
					vTh = V_TH;
					lambdaV = LAMBDA_V;
					tRefr = T_REFR;
					lambdaX = LAMBDA_X;
					alpha = ALPHA;
				}
            } else if (currentSection == "TOPOLOGY") {
				istringstream stream(line);
				string token;
				vector<pair<int, int>> packNeuronsType;
				int neuronType;
				int numTypeNeurons;
				
				stream >> token;
				if (token.compare("layer") == 0) {
					stream >> type;
				} else if (token.compare("neurons") == 0) {
					stream >> numNeurons;
				} else if (token.compare("type") == 0) {
					stream >> neuronType;
					if (neuronType < neuronParams.size()) {
						stream >> numTypeNeurons;
						neuronParams[neuronType].second = numTypeNeurons;
					} else {
						cout << "Error: Neuron type '" << neuronType << "' out of range." << endl;
						return -1;
					}
				} else if (token.compare("connections") == 0) {
					stream >> connections;
				} else if (token.compare("sparse_connection") == 0 && connections == "sparse") {
					int from, to;
					stream >> from >> to;
					sparseConnections.push_back(make_pair(from, to));
				} else if (token.compare("multisynaptic") == 0) {
					stream >> multisynaptic;
				} else {
					Layer currentLayer(type, numNeurons, neuronParams, connections, multisynaptic, sparseConnections);
					layers.push_back(currentLayer);

					for (int i = 0; i < neuronParams.size(); i++) neuronParams[i].second = 0;
					type = "undefined";
					numNeurons = -1;
					connections = "undefined";
					multisynaptic = -1;
					sparseConnections = {};
				}
			} else if (currentSection == "INPUT") {
                cout << "Processing INPUT section (not implemented)." << endl;
            }
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
		cout << "LAYER " << i << ": " << endl;
		cout << "Type: " << layers[i].getType() << endl;
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
			layers[i].feedForward(layers[i + 1], ct, 0.1);
			// layers[i + 1].updatePreSynapticTrace(layers[i], ct, 0.1);// ALPHA);
			// layers[i].propagateSpikes(layers[i + 1], ct);
		}
	}
}
