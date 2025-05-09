#include "../include/SNN.hpp"


SNN::SNN(): maxDelay(-1) {}

NeuronParameters::NeuronParameters(): vReset(0), vRest(0), v(0), vTh(0), lambdaV(0), tRefr(0), lambdaX(0), alpha(0) {}

int SNN::getTime() {
	return time;
}

int SNN::getDt() {
	return dt;
}

SNN::~SNN() {
    cout << "\nDestroying SNN" << endl;
    layers.clear();
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

	// cout << neuronsAux << endl;
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
		// if (topology.type == "input") {
		// 	inputSpikes.resize(topology.numNeurons);
		// }
		// Layer currentLayer(topology, neuronParams, dt);
		// layers.push_back(currentLayer);
		// layers.emplace_back(currentLayer);

		// layers.emplace_back(topology, neuronParams, dt);
		layers.emplace_back(make_shared<Layer>(topology, neuronParams, dt));

		
		for (int i = 0; i < neuronParams.size(); i++) neuronParams[i].second = 0;
		neuronsAux = 0;
        topology = {};
		// cout << "LLEEEEEEGAAAA" << endl;
    }
}

// void SNN::parseInput(const string &line) {
//     istringstream stream(line);
//     string neuronId;
//     stream >> neuronId;

//     // Validate format
//     if (neuronId[0] != 'N') {
//         cerr << "Error: Invalid neuron ID '" << neuronId << "' in INPUT section." << endl;
//         throw runtime_error("Invalid neuron ID");
//     }

//     deque<int> spikes;
//     int deltaTime, accumulatedTime = 0;
// 	unordered_set<int> timeBuckets;

// 	while (stream >> deltaTime) {
//         accumulatedTime += deltaTime;

//         int timeBucket = accumulatedTime / dt;

//         if (timeBuckets.find(timeBucket) != timeBuckets.end()) {
//             cerr << "Error: Spike at " << accumulatedTime << " ms for neuron '" << neuronId 
//                  << "' falls into the same interval as a previous spike (dt = " << dt << " ms)." << endl;
//             throw runtime_error("Multiple spikes in the same interval");
//         }

//         timeBuckets.insert(timeBucket);
//         spikes.push_back(accumulatedTime);
// 		// spikes.push_back(deltaTime);
//     }

//     if (spikes.empty()) {
//         cerr << "Error: No spikes found for neuron '" << neuronId << "'." << endl;
//         throw runtime_error("No spikes found");
//     }
	
// 	inputSpikes.push_back(spikes);
// }




void SNN::parseInput(const string &line) {
    istringstream stream(line);
    string neuronId;
    stream >> neuronId;

    // Validate format
    if (neuronId[0] != 'N') {
        cerr << "Error: Invalid neuron ID '" << neuronId << "' in INPUT section." << endl;
        throw runtime_error("Invalid neuron ID");
    }

    deque<int> spikes;
    int spikeTime = 0;
	int timeLeft = 0;
	int iterations = 0;

	while (stream >> spikeTime) {
		if (spikeTime - timeLeft <= 0) {
			cerr << "Error: Spike at " << spikeTime << " ms for neuron '" << neuronId 
				 << "' falls into the same interval as a previous spike (dt = " << dt << " ms)." << endl;
			cerr << iterations << " - " << timeLeft << " - " << spikeTime << endl;
			throw runtime_error("Multiple spikes in the same interval");
		} else {
			spikes.push_back(spikeTime);
		}

		iterations = ((spikeTime - timeLeft) / dt) + 1;
		timeLeft = (iterations * dt) - spikeTime;

	}

	// if (spikes.empty()) {
    //     cerr << "Error: No spikes found for neuron '" << neuronId << "'." << endl;
    //     throw runtime_error("No spikes found");
    // }
	
	inputSpikes.push_back(spikes);

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
	// layers.reserve(2);

	unordered_map<string, function<void(const string &)>> sectionHandlers = {
        {"PARAMETERS", [this](const string &line) { parseParameters(line); }},
        {"HYPERPARAMETERS", [this, &neuronDefaults](const string &line) { parseHyperparameters(line, neuronDefaults); }},
        {"TOPOLOGY", [this, &currentTopology, &neuronsAux](const string &line) { parseTopology(line, currentTopology, neuronsAux); }},
		{"INPUT", [this](const string &line) { parseInput(line); }},
    };

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
		layers[i]->setPresynapticLinks(*layers[i - 1]);
	}
}

void SNN::viewTopology() {
	setColor("blue"); cout << "\n-- NETWORK TOPOLOGY --" << endl; setColor("reset");
	for (int i = 0; i < layers.size(); i++) {
		setStyle("bold"); cout << "\nLAYER " << i << endl; setStyle("reset");
		cout << "Layer type: " << layers[i]->getType() << endl;
		cout << "Neuron model: " << layers[i]->getNumNeurons() << endl;
		cout << "Neural connections: " << layers[i]->getConnections() << endl;
		cout << "Multisynapses: " << layers[i]->getMultisynapses() << endl;
		if (layers[i]->getConnections() == "sparse") {
			cout << "Sparse connections: " << endl;
			for (int j = 0; j < layers[i]->getSparseConnections().size(); j++) {
				cout << layers[i]->getSparseConnections()[j].first << " -> " << layers[i]->getSparseConnections()[j].second << endl;
			}
		}
		layers[i]->getNeuronsType();
		// cout << endl;
	}
}

void SNN::viewInputSpikes() {
	setColor("blue"); cout << "\n-- INPUT SPIKES --\n" << endl; setColor("reset");
	cout << "Number of inputs: " << inputSpikes.size() << endl;
	for (int i = 0; i < inputSpikes.size(); i++) {
		cout << "- INPUT " << i << ": ";
		for (auto &spike : inputSpikes[i]) {
			cout << spike << " ";
		}
		cout << endl;
	}
}

// void SNN::trainNetwork() {
// 	int symTime = (time + maxDelay);
// 	int auxSD = 0;
// 	cout << "SymTime: " << symTime << endl;
// 	setColor("blue"); cout << "\n-- TRAINING NETWORK --" << endl; setColor("reset");
	
// 	for (int t = 0; t < symTime; t+=dt) {
// 		setStyle("bold"); setColor("green"); cout << "\nITERATION " << t/dt << endl; setStyle("reset");
// 		setStyle("bold"); cout << "Layer 0" << endl; setStyle("reset");
// 		for (int i = 0; i < inputSpikes.size(); i++) {
// 			if (!inputSpikes[i].empty() && inputSpikes[i].front() <= dt) {
// 				auxSD = inputSpikes[i].front();
// 				inputSpikes[i].pop_front();
// 				inputSpikes[i].front() -= auxSD;
// 				layers[0].getNeuron(i).setSpike(1);
// 				cout << " Neuron " << i << " spiked at time " << t << " ms" << endl;
// 			} else {
// 				inputSpikes[i].front() -= dt;
// 				layers[0].getNeuron(i).setSpike(0);
// 				cout << "Time Left: " << inputSpikes[i].front() << endl;
// 			}
// 		}
// 		for (int i = 1; i < layers.size(); i++) {	
// 			setStyle("bold"); cout << "Layer " << i << endl; setStyle("reset");
// 			layers[i].feedForward(i, t);
// 		}
// 	}
// 	setColor("blue"); cout << "\n-- TRAIN FINISHED --" << endl; setColor("reset");
// }



// void SNN::trainNetwork() {
// 	int symTime = (time + maxDelay);
// 	int auxSD = 0;
// 	vector<deque<int>> spikeTimes = inputSpikes;
// 	setColor("blue"); cout << "\n-- TRAINING NETWORK --" << endl; setColor("reset");
// 	cout << "SymTime: " << symTime << endl;
// 	// cout << inputSpikes.size() << endl;
// 	// cout << inputSpikes[0].size() << endl;
	
// 	for (int t = 0; t < symTime; t+=dt) {
// 		setStyle("bold"); setColor("green"); cout << "\nITERATION " << t/dt << endl; setStyle("reset");
// 		setStyle("bold"); cout << "Layer 0" << endl; setStyle("reset");
// 		for (int i = 0; i < inputSpikes.size(); i++) {
// 			if (!inputSpikes[i].empty()) {
// 				inputSpikes[i].front() -= dt;
// 				if (inputSpikes[i].front() <= 0) {
// 					auxSD = inputSpikes[i].front();
// 					inputSpikes[i].pop_front();
// 					inputSpikes[i].front() -= abs(auxSD);
// 					layers[0].getNeuron(i).setSpike(1);

// 					auxSD = spikeTimes[i].front();
// 					cout << " Neuron " << i << " spiked at time " << spikeTimes[i].front() << " ms" << endl;
// 					spikeTimes[i].pop_front();
// 					spikeTimes[i].front() += auxSD;
// 				} else {
// 					layers[0].getNeuron(i).setSpike(0);
// 				}
// 			}
// 		}
// 		for (int i = 1; i < layers.size(); i++) {	
// 			setStyle("bold"); cout << "Layer " << i << endl; setStyle("reset");
// 			layers[i].feedForward(i, t);
// 		}
// 	}
// 	setColor("blue"); cout << "\n-- TRAIN FINISHED --" << endl; setColor("reset");
// }



void SNN::trainNetwork() {
	int symTime = (time + maxDelay) / dt;
	// int symTime = (time + maxDelay);
	int auxSD = 0;
	vector<deque<int>> spikeTimes = inputSpikes;
	setColor("blue"); cout << "\n-- TRAINING NETWORK --" << endl; setColor("reset");
	cout << "SymTime: " << symTime << endl;
	// cout << inputSpikes.size() << endl;
	// cout << inputSpikes[0].size() << endl;
	
	for (int t = 0; t <= symTime; t++) {
		setStyle("bold"); setColor("green"); cout << "\nITERATION " << t << endl; setStyle("reset");
		setStyle("bold"); cout << "Layer 0" << endl; setStyle("reset");
		for (int i = 0; i < inputSpikes.size(); i++) {
			if (!inputSpikes[i].empty()) {
				inputSpikes[i].front() -= dt;
				if (inputSpikes[i].front() <= 0) {
					auxSD = inputSpikes[i].front();
					inputSpikes[i].pop_front();
					inputSpikes[i].front() -= abs(auxSD);
					layers[0]->getNeuron(i)->setSpike(1);

					auxSD = spikeTimes[i].front();
					cout << " Neuron " << i << " spiked at time " << spikeTimes[i].front() << " ms" << endl;
					spikeTimes[i].pop_front();
					spikeTimes[i].front() += auxSD;
				} else {
					layers[0]->getNeuron(i)->setSpike(0);
				}
			}
		}
		for (int i = 1; i < layers.size(); i++) {	
			setStyle("bold"); cout << "Layer " << i << endl; setStyle("reset");
			layers[i]->feedForward(i, t);
		}
		layers[0]->getNeuron(0)->setSpike(0);
	}
	setColor("blue"); cout << "\n-- TRAIN FINISHED --" << endl; setColor("reset");
}




void SNN::testNetwork() {
	cout << "-- TESTING NETWORK --" << endl;
}