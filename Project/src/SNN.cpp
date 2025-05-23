#include "../include/SNN.hpp"


SNN::SNN(): maxDelay(-1) {}

// NeuronParameters::NeuronParameters(): vReset(0), vRest(0), v(0), vTh(0), lambdaV(0), tRefr(0), lambdaX(0), alpha(0) {}

int SNN::getTime() {
	return time;
}

int SNN::getDt() {
	return dt;
}

SNN::~SNN() {
    cout << "Destroying SNN" << endl;
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
    else if (token == ">") neuronParams.push_back(neuron);
}

void SNN::parseTopology(const string &line, TopologyParameters &topology) {
    istringstream stream(line);
    string token;
    stream >> token;

	// int neuronType;
	// int numTypeNeurons;

    if (token == "layer") {
        stream >> topology.type;
    } else if (token == "neurons") {
        int aux;
        stream >> aux;
        if (aux > 0) {
            topology.height = aux;
            stream >> topology.width >> topology.channels;
        }
    } else if (token == "type") {
        stream >> topology.neuronType;
	} else if (token == "connections") {
        stream >> topology.connections;
    } else if (token == "kernel_size" && topology.connections == "sparse") {
        stream >> topology.r;
        topology.height = topology.height - topology.r + 1;
        topology.width = topology.width - topology.r + 1;
    } else if (token == "kernels_amount" && topology.connections == "sparse") {
        stream >> topology.k;
        topology.channels = topology.k;
    } else if (token == "multisynapses") {
        stream >> topology.multisynapses;
    } else if (token == "delay") {
        stream >> topology.delayMin >> topology.delayMax;
        if (topology.delayMin > topology.delayMax) {
            cerr << "Error: Invalid delay range." << endl;
            throw runtime_error("Invalid delay range");
        }
        maxDelay = max(maxDelay, topology.delayMax);
    } else if (token == ">") {
        topology.numNeurons = topology.height * topology.width * topology.channels;
        cout << "Creating layer " << topology.type << " with " << topology.numNeurons << " neurons." << endl;
		layers.emplace_back(make_shared<Layer>(topology, neuronParams[topology.neuronType], dt));

        // if (topology.connections == "sparse") {
        //     topology = {};
        // }
    }
}

// void SNN::parseInput(const string &line) {
//     istringstream stream(line);
//     string neuronId;
//     stream >> neuronId;

//     if (neuronId[0] != 'N') {
//         cerr << "Error: Invalid neuron ID '" << neuronId << "' in INPUT section." << endl;
//         throw runtime_error("Invalid neuron ID");
//     }

//     deque<int> spikes;
//     int spikeTime = 0;
// 	int timeLeft = 0;
// 	int iterations = 0;

// 	while (stream >> spikeTime) {
// 		if (spikeTime - timeLeft <= 0) {
// 			cerr << "Error: Spike at " << spikeTime << " ms for neuron '" << neuronId 
// 				 << "' falls into the same interval as a previous spike (dt = " << dt << " ms)." << endl;
// 			cerr << iterations << " - " << timeLeft << " - " << spikeTime << endl;
// 			throw runtime_error("Multiple spikes in the same interval");
// 		} else {
// 			spikes.push_back(spikeTime);
// 		}

// 		iterations = ((spikeTime - timeLeft) / dt) + 1;
// 		timeLeft = (iterations * dt) - spikeTime;

// 	}
	
// 	inputSpikes.push_back(spikes);

// }

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
	NeuronParameters neuronDefaults;
	TopologyParameters currentTopology;

	unordered_map<string, function<void(const string &)>> sectionHandlers = {
        {"PARAMETERS", [this](const string &line) { parseParameters(line); }},
        {"HYPERPARAMETERS", [this, &neuronDefaults](const string &line) { parseHyperparameters(line, neuronDefaults); }},
        {"TOPOLOGY", [this, &currentTopology](const string &line) { parseTopology(line, currentTopology); }},
		// {"INPUT", [this](const string &line) { parseInput(line); }},
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
                cout << " - Reading section: " << currentSection << endl;
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
	for (unsigned long i = 1; i < layers.size(); i++) {
		layers[i]->setPresynapticLinks(*layers[i - 1]);
	}
}

void SNN::viewTopology() {
	setColor("blue"); cout << "\n-- NETWORK TOPOLOGY --" << endl; setColor("reset");
	for (unsigned long i = 0; i < layers.size(); i++) {
		setStyle("bold"); cout << "\nLAYER " << i << endl; setStyle("reset");
		cout << " Layer: " << layers[i]->getType() << endl;
        cout << " Dimensions: " << layers[i]->getHeight() << " x " << layers[i]->getWidth() << " x " << layers[i]->getChannels() << endl;
		// cout << "Neuron model: " << layers[i]->getNumNeurons() << endl;
        cout << " Neuron type: " << layers[i]->getNeuronsType() << endl;
		cout << " Neural connections: " << layers[i]->getConnections() << endl;
        if (layers[i]->getConnections() == "sparse") {
			// cout << "Sparse connections: " << endl;
            cout << "  Windows size: " << layers[i]->getRDim() << " x " << layers[i]->getRDim() << endl;
            cout << "  Kernels: " << layers[i]->getChannels() << endl;
			// for (int j = 0; j < layers[i]->getSparseConnections().size(); j++) {
			// 	cout << layers[i]->getSparseConnections()[j].first << " -> " << layers[i]->getSparseConnections()[j].second << endl;
			// }
		}
		cout << " Multisynapses: " << layers[i]->getMultisynapses() << endl;
	}
}

// void SNN::viewInputSpikes() {
// 	setColor("blue"); cout << "\n-- INPUT SPIKES --\n" << endl; setColor("reset");
// 	cout << "Number of inputs: " << inputSpikes.size() << endl;
// 	for (int i = 0; i < inputSpikes.size(); i++) {
// 		cout << "- INPUT " << i << ": ";
// 		for (auto &spike : inputSpikes[i]) {
// 			cout << spike << " ";
// 		}
// 		cout << endl;
// 	}
// }





void readAedat(const string& filename, vector<DVSEvent>& eventsOut) {
    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << "\n";
        return;
    }

    while (true) {
        char c;
        file.get(c);
        if (!file) break;

        if (c != '#') {
            file.unget();
            break;
        }

        while (file.get(c) && c != '\n') {}
    }

    while (file) {
        AedatBlockHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        if ((unsigned long int)file.gcount() < sizeof(header)) break;

        if (header.eventType != 1 || header.eventValid == 0) {
            file.seekg(header.eventNumber * 8, ios::cur);
            continue;
        }

        for (uint32_t i = 0; i < header.eventValid; ++i) {
            DVSEvent evt;
            file.read(reinterpret_cast<char*>(&evt.data), sizeof(evt.data));
            file.read(reinterpret_cast<char*>(&evt.timestamp), sizeof(evt.timestamp));

            if (!file) break;
            eventsOut.push_back(evt);
        }
    }

    cout << eventsOut.size() << " events have been read.\n";
}

void readAnnotationsCSV(const string& filename, vector<GestureAnnotation>& annotationsOut) {
    ifstream file(filename);
    if (!file.is_open()) {
		cerr << "Error: The CSV file " << filename << " could not be opened.\n";
        return;
    }

    string line;

    if (!getline(file, line)) {
        cerr << "Error: The CSV file is empty.\n";
        return;
    }

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string item;

        GestureAnnotation ga;

        getline(ss, item, ',');
        if (item.empty() || !isdigit(item[0])) continue;
        ga.classLabel = stoi(item);

        getline(ss, item, ',');
        ga.startTime_usec = stoul(item);

        getline(ss, item, ',');
        ga.endTime_usec = stoul(item);

        annotationsOut.push_back(ga);
    }

    cout <<  annotationsOut.size() << " anotations have been read.\n";
}

// SpikeCube convertToSpikeCube(
//     const vector<DVSEvent>& events,
//     uint32_t startTime,
//     uint32_t endTime,
//     uint32_t dt, // timestep duration in µs
//     int width,
//     int height
// ) {
//     int timeSteps = (endTime - startTime) / dt + 1;

//     SpikeCube cube(timeSteps, vector<vector<uint8_t>>(height, vector<uint8_t>(width, 0)));

//     for (const auto& e : events) {
//         if (e.timestamp < startTime || e.timestamp > endTime)
//             continue;

//         int tIndex = (e.timestamp - startTime) / dt;
//         if (tIndex >= timeSteps || e.x() >= width || e.y() >= height)
//             continue;

//         cube[tIndex][e.y()][e.x()] = 1;
//     }


//     return cube;
// }

SpikeCubePolarity convertToSpikeCubesByPolarity(
    const std::vector<DVSEvent>& events,
    uint32_t startTime,
    uint32_t endTime,
    uint32_t dt,
    int width = 128,
    int height = 128
) {
    int timeSteps = (endTime - startTime) / dt + 1;
    SpikeCubePolarity cube;

    for (int t = 0; t < timeSteps; ++t) {
        std::vector<std::vector<uint8_t>> mapON(height, std::vector<uint8_t>(width, 0));
        std::vector<std::vector<uint8_t>> mapOFF(height, std::vector<uint8_t>(width, 0));
        cube.emplace_back(mapON, mapOFF);
    }

    for (const auto& e : events) {
        if (e.timestamp < startTime || e.timestamp > endTime) continue;

        unsigned long tIndex = (e.timestamp - startTime) / dt;
        if (tIndex >= cube.size() || e.x() >= width || e.y() >= height) continue;

        if (e.polarity()) {
            cube[tIndex].first[e.y()][e.x()] = 1;  // ON
        } else {
            cube[tIndex].second[e.y()][e.x()] = 1; // OFF
        }
    }

    return cube;
}

void SNN::trainNetwork() {
	string aedatFile = "./dataset/DvsGesture/user10_fluorescent_led.aedat";
    string csvFile = "./dataset/DvsGesture/user10_fluorescent_led_labels.csv";

    vector<DVSEvent> events;
    vector<GestureAnnotation> annotations;

    int indexON = 0;
    int indexOFF = 0;

    setColor("blue"); cout << "\n-- TRAINING NETWORK --\n" << endl; setColor("reset");

    readAedat(aedatFile, events);
    readAnnotationsCSV(csvFile, annotations);

    // Choose the first gesture for training
    // GestureAnnotation gesture = annotations[0];
    // uint32_t dt = 100; // 1ms

    for (unsigned long i = 0; i < annotations.size(); ++i) {
    // for (int i = 0; i < 1; ++i) {
        GestureAnnotation gesture = annotations[i];
        size_t count = count_if(events.begin(), events.end(), [&](const DVSEvent& evt) {
                return evt.timestamp >= gesture.startTime_usec && evt.timestamp <= gesture.endTime_usec;
            });
        // cout << "Gesture " << i + 1
        //           << " (class " << gesture.classLabel << ") from "
        //           << gesture.startTime_usec << " to "
        //           << gesture.endTime_usec << " usec.\n";
        cout << "Gesture " << i + 1
                    << " (class " << gesture.classLabel << ") has "
                    << count << " events.\n";

        // SpikeCube spikeData = convertToSpikeCube(events, gesture.startTime_usec, gesture.endTime_usec, dt);
        SpikeCubePolarity spikeData = convertToSpikeCubesByPolarity(events, gesture.startTime_usec, gesture.endTime_usec, dt);


        cout << "Processing spike map with "
                << spikeData.size() << " time steps.\n";

        for (size_t t = 0; t < spikeData.size(); ++t) {
            auto& mapON = spikeData[t].first;
            auto& mapOFF = spikeData[t].second;

            for (int y = 0; y < 128; ++y) {
                for (int x = 0; x < 128; ++x) {
                    indexON = ((y * 128) + x);
                    indexOFF = ((128 * 128) + (y * 128) + x);

                    layers[0]->getNeuron(indexON)->setSpike(mapON[y][x]);

                    layers[0]->getNeuron(indexOFF)->setSpike(mapOFF[y][x]);
                }
            }

            for (unsigned long index = 1; index < layers.size(); index++) {
                // setStyle("bold"); cout << "Layer " << i << endl; setStyle("reset");
                layers[index]->feedForward(index, t);
                // layers[i]->visualizeSpikes(t);
            }
            if (t % 50 == 0) cout << "Time step " << t << endl;
        }
        layers[2]->showSpikeHistory();
    }
}


// void SNN::trainNetwork() {
// 	int symTime = (time + maxDelay) / dt;
// 	// int symTime = (time + maxDelay);
// 	int auxSD = 0;
// 	vector<deque<int>> spikeTimes = inputSpikes;
// 	setColor("blue"); cout << "\n-- TRAINING NETWORK --" << endl; setColor("reset");
// 	cout << "SymTime: " << symTime << endl;
// 	// cout << inputSpikes.size() << endl;
// 	// cout << inputSpikes[0].size() << endl;
	
// 	for (int t = 0; t <= symTime; t++) {
// 		setStyle("bold"); setColor("green"); cout << "\nITERATION " << t << endl; setStyle("reset");
// 		setStyle("bold"); cout << "Layer 0" << endl; setStyle("reset");
// 		for (int i = 0; i < inputSpikes.size(); i++) {
// 			if (!inputSpikes[i].empty()) {
// 				inputSpikes[i].front() -= dt;
// 				if (inputSpikes[i].front() <= 0) {
// 					auxSD = inputSpikes[i].front();
// 					inputSpikes[i].pop_front();
// 					inputSpikes[i].front() -= abs(auxSD);
// 					layers[0]->getNeuron(i)->setSpike(1);

// 					auxSD = spikeTimes[i].front();
// 					cout << " Neuron " << i << " spiked at time " << spikeTimes[i].front() << " ms" << endl;
// 					spikeTimes[i].pop_front();
// 					spikeTimes[i].front() += auxSD;
// 				} else {
// 					layers[0]->getNeuron(i)->setSpike(0);
// 				}
// 			}
// 		}
// 		for (int i = 1; i < layers.size(); i++) {	
// 			setStyle("bold"); cout << "Layer " << i << endl; setStyle("reset");
// 			layers[i]->feedForward(i, t);
// 		}
// 		layers[0]->getNeuron(0)->setSpike(0);
// 	}
// 	setColor("blue"); cout << "\n-- TRAIN FINISHED --" << endl; setColor("reset");
// }






void SNN::testNetwork() {
	cout << "-- TESTING NETWORK --" << endl;
}