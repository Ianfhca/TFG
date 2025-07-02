/**
 * @file SNN.cpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Implements the SNN class for simulating spiking neural networks.
 */

#include "../include/SNN.hpp"

SNN::SNN(): maxDelay(-1) {}

SNN::~SNN() {
    cout << "Destroying SNN" << endl;
    layers.clear();
}

void SNN::parseParameters(const string &line) {
    istringstream stream(line);
    string token;
    stream >> token;

    if (token == "time") {
        stream >> time >> timeUnit;
    } else if (token == "dt") {
        stream >> dt >> dtUnit;
    } else if (token == ">") {
        if (dtUnit != "us") {
            dt = convertTime(dt, dtUnit, "us");
            dtUnit = "us";
        }
        time = convertTime(time, timeUnit, dtUnit);
        if (time == -1) {
            cerr << "Error: Invalid time conversion." << endl;
            throw runtime_error("Invalid time conversion");
        }
		cout << "Time: " << static_cast<unsigned long>(time) << " " << dtUnit << endl;
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
    else if (token == "tau_m") stream >> neuron.tauM;
    else if (token == "t_refr") stream >> neuron.tRefr >> tRefrUnit;
    else if (token == "lambda_x") stream >> neuron.lambdaX;
    else if (token == "alpha") stream >> neuron.alpha;
    else if (token == "learning_rate") stream >> neuron.learningRate;
    else if (token == "a") stream >> neuron.aValue;
    else if (token == "convergence_th") stream >> neuron.convergenceTh;
    else if (token == ">") {
        if (tRefrUnit != "us") neuron.tRefr = convertTime(neuron.tRefr, tRefrUnit, dtUnit);
        neuronParams.push_back(neuron);
    }
}

void SNN::parseTopology(const string &line, TopologyParameters &topology) {
    istringstream stream(line);
    string token;
    stream >> token;

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
    } else if (token == "kernel_size" && topology.connections == "local") {
        stream >> topology.r;
        topology.height = topology.height - topology.r + 1;
        topology.width = topology.width - topology.r + 1;
    } else if (token == "kernels_amount" && topology.connections == "local") {
        stream >> topology.k;
        topology.channels = topology.k;
    } else if (token == "sparse_connection" && topology.connections == "sparse") {
        int from, to;
        stream >> from >> to;
        topology.sparseConnections.push_back({from, to});
    } else if (token == "multisynapses") {
        stream >> topology.multisynapses;
    } else if (token == "stdp") {
        stream >> topology.stdpRule;
        cout << topology.type << " " << topology.stdpRule << endl;
    } else if (token == "wta") {
        stream >> topology.wtaRule;
        cout << topology.type << " " << topology.wtaRule << endl;
    } else if (token == "delay") {
        stream >> topology.delayMin >> topology.delayMax >> delayUnit;
        if (topology.delayMin > topology.delayMax) {
            cerr << "Error: Invalid delay range." << endl;
            throw runtime_error("Invalid delay range");
        }
        maxDelay = max(maxDelay, topology.delayMax);
    } else if (token == ">") {
        topology.numNeurons = topology.height * topology.width * topology.channels;
        if (topology.type == "Input") {
            height = topology.height;
            width = topology.width;
            channels = topology.channels;
        }
        if (delayUnit != dtUnit) {
            topology.delayMin = convertTime(topology.delayMin, delayUnit, dtUnit);
            topology.delayMax = convertTime(topology.delayMax, delayUnit, dtUnit);
        }
        topology.mode = mode;
        cout << "Creating layer " << topology.type << " with " << topology.numNeurons << " neurons." << endl;
		layers.emplace_back(make_shared<Layer>(topology, neuronParams[topology.neuronType], dt));
    }
}

int SNN::initNetwork(char &filename, string &nMode) {
	string currentSection, line;
	NeuronParameters neuronDefaults;
	TopologyParameters currentTopology;
    mode = nMode;
    topologyFile = &filename;
    string aux = "./topology/" + topologyFile + ".txt";
    char* file = aux.data();

    string outputFolder = "./output/";
    if (!filesystem::exists(outputFolder)) {
        filesystem::create_directories(outputFolder);
    }

    baseName = outputFolder + topologyFile + "/";
    if (!filesystem::exists(baseName)) {
        filesystem::create_directories(baseName);
    }

	unordered_map<string, function<void(const string &)>> sectionHandlers = {
        {"PARAMETERS", [this](const string &line) { parseParameters(line); }},
        {"HYPERPARAMETERS", [this, &neuronDefaults](const string &line) { parseHyperparameters(line, neuronDefaults); }},
        {"TOPOLOGY", [this, &currentTopology](const string &line) { parseTopology(line, currentTopology); }},
    };

	ifstream network_file(file);
	if (!network_file.is_open()) {
		cerr << "Error: Unable to open file " << file << endl;
		return 110;
	} else {
		cout << "Reading file " << file << endl;
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
        if (layers[i]->getConnections() == "local") {
            cout << "  Windows size: " << layers[i]->getRDim() << " x " << layers[i]->getRDim() << endl;
            cout << "  Kernels: " << layers[i]->getChannels() << endl;
		} else if (layers[i]->getConnections() == "sparse") {
			cout << " Sparse connections: " << endl;
			for (unsigned long j = 0; j < layers[i]->getSparseConnections().size(); j++) {
				cout << "  " << layers[i]->getSparseConnections()[j].first << " -> " << layers[i]->getSparseConnections()[j].second << endl;
			}
		}
		cout << " Multisynapses: " << layers[i]->getMultisynapses() << endl;
        // cout << " Delay range: " << layers[i]->getDelayMin() << " - " << layers[i]->getDelayMax() << endl;
	}
}

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

SpikeCubePolarity convertToSpikeCubesByPolarity(const vector<DVSEvent>& events, uint32_t startTime, uint32_t endTime, uint32_t dt, int width = 128, int height = 128) {
    int timeSteps = (endTime - startTime) / dt + 1;
    SpikeCubePolarity cube;

    for (int t = 0; t < timeSteps; ++t) {
        vector<vector<uint8_t>> mapON(height, vector<uint8_t>(width, 0));
        vector<vector<uint8_t>> mapOFF(height, vector<uint8_t>(width, 0));
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

vector<string> SNN::readTrialsList(const string& filename) {
    vector<string> trials;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << endl;
        return trials;
    }
    
    string line;
    while (getline(file, line)) {
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        if (!line.empty() && line[0] != '#') {
            trials.push_back(line);
        }
    }
    
    cout << trials.size() << " trials loaded for training." << endl;
    return trials;
}

string SNN::generateCSVFilename(const string& aedatFilename) {
    string csvFilename = aedatFilename;
    
    size_t pos = csvFilename.find(".aedat");
    if (pos != string::npos) {
        csvFilename.replace(pos, 6, "_labels.csv");
    }
    
    return csvFilename;
}

int SNN::processGestureData(const SpikeCubePolarity& spikeData, int gestureClass, unsigned long& symTime, unsigned long symCap) {
    int indexON = 0;
    int indexOFF = 0;
    
    for (size_t t = 0; t < spikeData.size(); ++t) {
        const auto& mapON = spikeData[t].first;
        const auto& mapOFF = spikeData[t].second;

        if (symTime >= symCap) {
            cout << "Stopping training at time step " << t << " due to symmetric time limit.\n";
            return -1;
        }

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                indexON = ((y * height) + x);
                indexOFF = ((height * width) + (y * height) + x);

                layers[0]->getNeuron(indexON)->setSpike((int)mapON[y][x]);
                layers[0]->getNeuron(indexOFF)->setSpike((int)mapOFF[y][x]);
            }
        }

        // // Visualize spikes at each time step
        // if (t % 1000 == 0) {
        //     cout << "Time step " << t << endl;
        //     layers[layers.size()-1]->showSpikeHistory();
        // }
        
        for (unsigned long index = 1; index < layers.size(); index++) {
            layers[index]->feedForward(baseName + mode, gestureClass, (int)symTime);
        }
        symTime++;
    }
    return 0;
}

void SNN::trainNetwork() {
    setColor("blue"); cout << "\n-- TRAINING NETWORK --\n" << endl; setColor("reset");

    string trialsFile = "./dataset/DvsGesture/trials_to_train.txt";
    vector<string> trialFiles = readTrialsList(trialsFile);
    
    if (trialFiles.empty()) {
        cerr << "Error: No training files found in " << trialsFile << endl;
        return;
    }

    unsigned long symCap = 0;
    if (time < 1) symCap = numeric_limits<unsigned long>::max();
    else symCap = (time + maxDelay) / dt;
    unsigned long symTime = 0;

    cout << "Symmetric time: " << symCap << " steps (dt = " << dt << " us)\n";

    if (filesystem::exists(baseName + mode + ".txt")) {
        filesystem::remove(baseName + mode + ".txt");
    }

    for (const string& trialFile : trialFiles) {
        string aedatFile = "./dataset/DvsGesture/" + trialFile;
        string csvFile = "./dataset/DvsGesture/" + generateCSVFilename(trialFile);
        
        cout << "\n=== Processing trial: " << trialFile << " ===" << endl;
        
        vector<DVSEvent> events;
        vector<GestureAnnotation> annotations;

        readAedat(aedatFile, events);
        readAnnotationsCSV(csvFile, annotations);

        for (unsigned long i = 0; i < annotations.size(); ++i) {
            GestureAnnotation gesture = annotations[i];
            size_t count = count_if(events.begin(), events.end(), [&](const DVSEvent& evt) {
                return evt.timestamp >= gesture.startTime_usec && evt.timestamp <= gesture.endTime_usec;
            });

            cout << "Gesture " << i + 1 << " (class " << gesture.classLabel << ") has " << count << " events." << endl;

            SpikeCubePolarity spikeData = convertToSpikeCubesByPolarity(events, gesture.startTime_usec, gesture.endTime_usec, dt, height, width);

            cout << "Processing spike map with " << spikeData.size() << " time steps." << endl;

            if(processGestureData(spikeData, gesture.classLabel, symTime, symCap) == -1) return;
        }        
    }

    cout << "\n=== Training completed on all trials ===" << endl;
}

void SNN::saveWeights() {
    for (unsigned long i = 1; i < layers.size(); i++) {
        layers[i]->saveWeights(baseName, i);
    }
}

int SNN::loadWeights() {
    for (unsigned long i = 1; i < layers.size(); i++) {
        if (layers[i]->loadWeights(baseName, i) != 0) return 1;
        else cout << "Loaded weights for layer " << i << " (" << layers[i]->getType() << ")." << endl;
    }

    return 0;
}


void SNN::testNetwork() {
    setColor("blue"); cout << "\n-- TESTING NETWORK --\n" << endl; setColor("reset");

    string trialsFile = "./dataset/DvsGesture/trials_to_test.txt";
    vector<string> trialFiles = readTrialsList(trialsFile);
    
    if (trialFiles.empty()) {
        cerr << "Error: No testing files found in " << trialsFile << endl;
        return;
    }

    unsigned long symCap = 0;
    if (time < 1) symCap = numeric_limits<unsigned long>::max();
    else symCap = (time + maxDelay) / dt;
    unsigned long symTime = 0;

    cout << "Symmetric time: " << symTime << " steps (dt = " << dt << " us)\n";

    if (filesystem::exists(baseName + mode + ".txt")) {
        filesystem::remove(baseName + mode + ".txt");
    }

    for (const string& trialFile : trialFiles) {
        string aedatFile = "./dataset/DvsGesture/" + trialFile;
        string csvFile = "./dataset/DvsGesture/" + generateCSVFilename(trialFile);
        
        cout << "\n=== Processing trial: " << trialFile << " ===" << endl;
        
        vector<DVSEvent> events;
        vector<GestureAnnotation> annotations;

        readAedat(aedatFile, events);
        readAnnotationsCSV(csvFile, annotations);

        for (unsigned long i = 0; i < annotations.size(); ++i) {
            GestureAnnotation gesture = annotations[i];
            size_t count = count_if(events.begin(), events.end(), [&](const DVSEvent& evt) {
                return evt.timestamp >= gesture.startTime_usec && evt.timestamp <= gesture.endTime_usec;
            });

            cout << "Gesture " << i + 1 << " (class " << gesture.classLabel << ") has " << count << " events." << endl;

            SpikeCubePolarity spikeData = convertToSpikeCubesByPolarity(events, gesture.startTime_usec, gesture.endTime_usec, dt, height, width);

            cout << "Processing spike map with " << spikeData.size() << " time steps." << endl;

            if(processGestureData(spikeData, gesture.classLabel, symTime, symCap) == -1) return;
        }
    }
    
    cout << "\n=== Testing completed on all trials ===" << endl;
}