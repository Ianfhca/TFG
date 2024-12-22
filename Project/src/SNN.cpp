#include "../include/SNN.h"

SNN::SNN(): maxDelay(-1) {}

NeuronParameters::NeuronParameters(): vReset(0), vRest(0), v(0), vTh(0), lambdaV(0), tRefr(0), lambdaX(0), alpha(0) {}

int SNN::getTime() {
	return time;
}

int SNN::getDt() {
	return dt;
}

void SNN::parseParameters(const string &line) {
    istringstream stream(line);
    string token;
    stream >> token;

	double timeAux;

    if (token == "time") {
        stream >> timeAux >> timeUnit;
    } else if (token == "dt") {
        stream >> dt >> dtUnit;
    } else if (token == ">") {
        time = convertTime(timeAux, timeUnit, dtUnit);
        if (time == -1) {
            cerr << "Error: Invalid time conversion." << endl;
            throw runtime_error("Invalid time conversion");
        }
		// cout << "Time: " << time << " " << dtUnit << endl;
    }
}

void SNN::parseHyperparameters(const string &line, NeuronParameters &neuron) {
    istringstream stream(line);
    string token;
    stream >> token;

    if (token == "v_reset") stream >> neuron.vReset;
    else if (token == "v_rest") stream >> neuron.vRest;
    else if (token == "v") stream >> neuron.v;
    else if (token == "v_th") stream >> neuron.vTh;
    else if (token == "lambda_v") stream >> neuron.lambdaV;
    else if (token == "t_refr") stream >> neuron.tRefr;
    else if (token == "lambda_x") stream >> neuron.lambdaX;
    else if (token == "alpha") stream >> neuron.alpha;
    else if (token == ">") neuronParams.push_back({neuron, 0});
}

void SNN::parseTopology(const string &line, TopologyParameters &topology, int &neuronsAux) {
    istringstream stream(line);
    string token;
    stream >> token;

	int neuronType;
	int numTypeNeurons;

	cout << neuronsAux << endl;
	// cout << neuronParams.size() << endl;

    if (token == "layer") stream >> topology.type;
    else if (token == "neurons") stream >> topology.numNeurons;
	else if (token == "type") {
		stream >> neuronType;
		stream >> numTypeNeurons;
		if (neuronType >= neuronParams.size()) {
			cerr << "Error: Neuron type '" << neuronType << "' out of range." << endl;
			throw runtime_error("Neuron type out of range");
		} else if (neuronsAux + numTypeNeurons > topology.numNeurons) {
			cerr << "Error: Neuron type '" << neuronType << "' on " << topology.type << " layer exceeds the sum of neurons '"<< topology.numNeurons << "'." << endl;
			throw runtime_error("Neuron type exceeds the number of neurons");	
		}
		neuronParams[neuronType].second += numTypeNeurons;
		neuronsAux += numTypeNeurons;
	} else if (token == "connections") stream >> topology.connections;
    else if (token == "multisynapses") stream >> topology.multisynapses;
    else if (token == "delay") {
        stream >> topology.delayMin >> topology.delayMax;
        if (topology.delayMin > topology.delayMin) {
            cerr << "Error: Invalid delay range." << endl;
            throw runtime_error("Invalid delay range");
        }
        maxDelay = max(maxDelay, topology.delayMax);
    } else if (token == "sparse_connection" && topology.connections == "sparse") {
        int from, to;
        stream >> from >> to;
        topology.sparseConnections.push_back({from, to});
    } else if (token == ">") {
		if (neuronsAux != topology.numNeurons) {
			cerr << "Error: The " << topology.type << " layer's number of neurons does not match the sum of neurons '"<< topology.numNeurons << "'." << endl;
			throw runtime_error("Number of neurons does not match the sum of neuron types");
		}
		Layer currentLayer(topology, neuronParams, dt);
		layers.push_back(currentLayer);
		for (int i = 0; i < neuronParams.size(); i++) neuronParams[i].second = 0;
		neuronsAux = 0;
        topology = {};
    }
}

/**
 * @brief  This function initializes the network topology from a file
 * 
 * This function reads a file which contains the network topology and initializes it based on the information provided inside.
 * The file is structured by sections and blocks. Each section contains specific information about network.
 * 
 * Composed sections are:
 * 
 * -- PARAMETERS -- -> Contains the parameters of the neurons
 * -- TOPOLOGY -- -> Contains the information about the layers
 * -- INPUT -- -> Contains the standard input for the network
 * 
 * Also there are some rules to follow:
 * 
 * > -> Separetes blocks beteween sections (Required)
 * # -> Comments
 * White spaces are ignored
 * 
 * @param  file:  The file to read the network topology from
 * 
 * @return 0 if successful, 110 if file cannot be opened and 1 otherwise
 */
int SNN::initNetwork(char &file) {
	string currentSection, line;
	int neuronsAux = 0;
	NeuronParameters neuronDefaults;
	TopologyParameters currentTopology;

	unordered_map<string, function<void(const string &)>> sectionHandlers = {
        {"PARAMETERS", [this](const string &line) { parseParameters(line); }},
        {"HYPERPARAMETERS", [this, &neuronDefaults](const string &line) { parseHyperparameters(line, neuronDefaults); }},
        {"TOPOLOGY", [this, &currentTopology, &neuronsAux](const string &line) { parseTopology(line, currentTopology, neuronsAux); }},
    };

	// Read network file
	ifstream network_file(&file);
	if (!network_file.is_open()) {
		cerr << "Error: Unable to open file " << &file << endl;
		return 110;
	} else {
		cout << "Reading file " << &file << endl;
		while (getline(network_file, line)) {

			if (line.length() == 0 || line[0] == '#') continue;

			 if (line.rfind("--", 0) == 0) {
                currentSection = line.substr(3, line.size() - 6);
                cout << "Entering section: " << currentSection << endl;
                continue;
            }

			if (sectionHandlers.find(currentSection) != sectionHandlers.end()) {
				sectionHandlers[currentSection](line);
			} else {
				cerr << "Error: Invalid section '" << currentSection << "'." << endl;
				return 1;
			}

		}
		network_file.close();
	}

	return 0;
};

/**
 * @brief  Brief description of this function
 * 
 * Long description
 * 
 * @param  value1:  description
 * @param  value2:  description
 * 
 * @return description
 */
void SNN::linkLayers() {
	for (int i = 1; i < layers.size(); i++) {
		// layers[i].setPostsynapticLinks(layers[i + 1]);
		layers[i].setPresynapticLinks(layers[i - 1]);
	}
}

void SNN::viewTopology() {
	cout << "-- NETWORK TOPOLOGY --" << endl;
	for (int i = 0; i < layers.size(); i++) {
		cout << "LAYER " << i << ": " << endl;
		cout << "Type: " << layers[i].getType() << endl;
		cout << "Neurons: " << layers[i].getNumNeurons() << endl;
		cout << "Connections: " << layers[i].getConnections() << endl;
		cout << "Multisynapses: " << layers[i].getMultisynapses() << endl;
		if (layers[i].getConnections() == "sparse") {
			cout << "Sparse connections: " << endl;
			for (int j = 0; j < layers[i].getSparseConnections().size(); j++) {
				cout << layers[i].getSparseConnections()[j].first << " -> " << layers[i].getSparseConnections()[j].second << endl;
			}
		}
		layers[i].getNeuronsType();
		cout << endl;
	}
}

void SNN::trainNetwork() {
	int symTime = (time + maxDelay);
	cout << "SymTime: " << symTime << endl;
	cout << "-- TRAINING NETWORK --" << endl;
	
	for (int t = 0; t < symTime; t+=dt) {
		cout << "- Layer " << 0 << " -" << endl;
		// Check this tratar la capa de entrada
		for (int i = 1; i < layers.size(); i++) {	
			cout << "- Layer " << i << " -" << endl;
			layers[i].feedForward(t);
		}
	}
	cout << "-- THE END --" << endl;
}

void SNN::testNetwork() {
	cout << "-- TESTING NETWORK --" << endl;
}


// int SNN::initNetwork(char &file) {

// 	maxDelay = 0;
// 	double timeAux = TIME;
// 	int dtAux = DT;
// 	string timeUnit = TIME_UNIT;
// 	string dtUnit = DT_UNIT;

// 	double vReset = V_RESET;
// 	double vRest = V_REST;
// 	double v = V;
// 	double vTh = V_TH;
// 	double lambdaV = LAMBDA_V;
// 	double tRefr = T_REFR;
// 	double lambdaX = LAMBDA_X;
// 	double alpha = ALPHA;
// 	vector<pair<vector<double>, int>> neuronParams;

// 	string type = UNDEFINED;
//     int numNeurons = NONE;
//     string connections = UNDEFINED;
// 	int multisynapses = NONE;
// 	pair<int, int> delayRange = {MIN_DELAY, MAX_DELAY};
//     vector<pair<int, int>> sparseConnections;

// 	string currentSection, line;

// 	// Read network file
// 	ifstream network_file(&file);
// 	if (!network_file.is_open()) {
// 		cout << "Error: Unable to open file " << &file << endl;
// 		return -1;
// 	} else {
// 		cout << "Reading file " << &file << endl;
// 		while (getline(network_file, line)) {

// 			if (line.length() == 0 || line[0] == '#') continue;

// 			 if (line.rfind("--", 0) == 0) {
//                 currentSection = line.substr(3, line.size() - 6);
//                 cout << "Entering section: " << currentSection << endl;
//                 continue;
//             }

// 			if (currentSection == "PARAMETERS") {
//                 istringstream stream(line);
// 				string token;
				
// 				stream >> token;

// 				if (token.compare("time") == 0) {
// 					stream >> timeAux;
// 					stream >> timeUnit;
// 					// cout << "Time: " << time << " " << timeUnit << endl;
// 				} else if (token.compare("dt") == 0) {
// 					stream >> dtAux;
// 					stream >> dtUnit;
// 				} else if (token.compare(">") == 0) {
// 					time = convertTime(timeAux, timeUnit, dtUnit);
// 					dt = dtAux;
// 					if (time == -1) {
// 						cout << "Error: Invalid time conversion." << endl;
// 						return -1;
// 					}
// 					// cout << "Time: " << time << " " << dtUnit << endl;
// 					// cout << "dt: " << dt << " " << dtUnit << endl;
// 				}
// 			} else if (currentSection == "HYPERPARAMETERS") {
//                 istringstream stream(line);
// 				string token;
				
// 				stream >> token;
				
// 				if (token.compare("v_reset") == 0) {
// 					stream >> vReset;
// 				} else if (token.compare("v_rest") == 0) {
// 					stream >> vRest;
// 				} else if (token.compare("v") == 0) {
// 					stream >> v;
// 				} else if (token.compare("v_th") == 0) {
// 					stream >> vTh;
// 				} else if (token.compare("lambda_v") == 0) {
// 					stream >> lambdaV;
// 				} else if (token.compare("t_refr") == 0) {
// 					stream >> tRefr;
// 				} else if (token.compare("lambda_x") == 0) {
// 					stream >> lambdaX;
// 				} else if (token.compare("alpha") == 0) {
// 					stream >> alpha;
// 				} else if (token.compare(">") == 0) {
// 					neuronParams.push_back(make_pair(vector<double>{vReset, vRest, v, vTh, lambdaV, tRefr, lambdaX, alpha}, 0));

// 					vReset = V_RESET;
// 					vRest = V_REST;
// 					v = V;
// 					vTh = V_TH;
// 					lambdaV = LAMBDA_V;
// 					tRefr = T_REFR;
// 					lambdaX = LAMBDA_X;
// 					alpha = ALPHA;
// 				}
//             } else if (currentSection == "TOPOLOGY") {
// 				istringstream stream(line);
// 				string token;
// 				vector<pair<int, int>> packNeuronsType;
// 				int neuronType;
// 				int numTypeNeurons;
// 				int neuronsAux = 0;
				
// 				stream >> token;
// 				if (token.compare("layer") == 0) {
// 					stream >> type;
// 				} else if (token.compare("neurons") == 0) {
// 					stream >> numNeurons;
// 				} else if (token.compare("type") == 0) {
// 					stream >> neuronType;
// 					if (neuronType < neuronParams.size()) {
// 						stream >> token;
// 						stream >> numTypeNeurons;
// 						if (neuronsAux + numTypeNeurons <= numNeurons) {
// 							neuronParams[neuronType].second = numTypeNeurons;
// 							neuronsAux += numTypeNeurons;
// 						} else {
// 							cout << "Error: Neuron type '" << neuronType << "' out of number of neurons range '"<< numTypeNeurons << "'." << endl;
// 							return -1;
// 						}
// 					} else {
// 						cout << "Error: Neuron type '" << neuronType << "' out of range." << endl;
// 						return -1;
// 					}
// 				} else if (token.compare("connections") == 0) {
// 					stream >> connections;
// 				} else if (token.compare("sparse_connection") == 0 && connections == "sparse") {
// 					int from, to;
// 					stream >> from >> to;
// 					sparseConnections.push_back(make_pair(from, to));
// 				} else if (token.compare("multisynapses") == 0) {
// 					stream >> multisynapses;
// 				} else if (token.compare("delay") == 0) {
// 					stream >> delayRange.first;
// 					stream >> delayRange.second;
// 					if (delayRange.first > delayRange.second) {
// 						cout << "Error: Invalid delay range." << endl;
// 						return -1;
// 					}
// 					if (delayRange.second > maxDelay) maxDelay = delayRange.second;
// 				} else if (token.compare(">") == 0) {
// 					Layer currentLayer(type, numNeurons, neuronParams, connections, multisynapses, delayRange, sparseConnections);
// 					layers.push_back(currentLayer);

// 					for (int i = 0; i < neuronParams.size(); i++) neuronParams[i].second = 0;
// 					type = "undefined";
// 					numNeurons = -1;
// 					connections = "undefined";
// 					multisynapses = -1;
// 					sparseConnections = {};
// 					neuronsAux = 0;
// 				}
// 			} else if (currentSection == "INPUT") {
//                 cout << "Processing INPUT section (not implemented)." << endl;
//             }
// 		}
// 		network_file.close();
// 	}

// 	if (maxDelay == 0) maxDelay = MAX_DELAY;
// 	cout << "Max delay: " << maxDelay << endl;
// 	// linkLayers();

// 	return 0;
// };