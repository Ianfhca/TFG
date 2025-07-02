#include "../include/Layer.hpp"

Layer::Layer(const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt)
    : type(topology.type), height(topology.height), width(topology.width), channels(topology.channels),
      numNeurons(topology.numNeurons), neuronType(topology.neuronType), connections(topology.connections),
      rDim(topology.r), multisynapses(topology.multisynapses), sparseConnections(topology.sparseConnections) {

    int neuronIdx = 0;
    // if (type == "Output") spikeHistory.resize(numNeurons, 0);

    for (int ch = 0; ch < channels; ch++) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                neuronIdx = ((ch * height * width) + (i * width) + j);
                vector<int> neuronId = {i, j, ch, neuronIdx};
                neurons.emplace_back(make_shared<LIFneuron>(neuronId, topology, neuronParams, dt));
            }
        }
    }
}

Layer::~Layer() {
    // cout << "-----------------------" << endl;
    // cout << "Destroying Layer " << type << endl;
    neurons.clear();
}

string Layer::getType() {
    return type;
};

int Layer::getHeight() {
    return height;
};

int Layer::getWidth() {
    return width;
};

int Layer::getChannels() {
    return channels;
};

int Layer::getNumNeurons() {
    return numNeurons;
};

int Layer::getNeuronsType() {
    return neuronType;
};

string Layer::getConnections() {
    return connections;
};

int Layer::getRDim() {
    return rDim;
};

int Layer::getMultisynapses() {
    return multisynapses;
};

shared_ptr<LIFneuron> Layer::getNeuron(int i) {
    return neurons[i];
};

vector<pair<int, int>>& Layer::getSparseConnections() {
    return sparseConnections;
};

void Layer::setPresynapticLinks(Layer &preLayer) {
    if (connections == "local") {
        int preHeight = preLayer.getHeight();
        int preWidth = preLayer.getWidth();
        int preChannels = preLayer.getChannels();

        for (int ch = 0; ch < channels; ch++) {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int neuronIdx = ((ch * height * width) + (i * width) + j);
                    for (int preCh = 0; preCh < preChannels; preCh++) {
                        for (int y = 0; y < rDim; y++) {
                            for (int x = 0; x < rDim; x++) { // 2 options here
                                // Option 1: Calculous for the current window view centered on the current neuron
                                // int preY = i + y - rDim / 2;
                                // int preX = j + x - rDim / 2;

                                // Option 2: Sliding window
                                int preY = i + y;
                                int preX = j + x;
                                if (preY >= 0 && preY < preHeight && preX >= 0 && preX < preWidth) {
                                    int preNeuronIdx = ((preCh * preHeight * preWidth) + (preY * preWidth) + preX);
                                    if (preNeuronIdx < preLayer.getNumNeurons()) {
                                        neurons[neuronIdx]->setPresynapticLinks(preLayer.neurons[preNeuronIdx], preLayer.getNumNeurons());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (connections == "sparse") {
        int preNeuronIdx = 0; 
        int neuronIdx = 0;

        for (unsigned long i = 0; i < sparseConnections.size(); i++) {
            preNeuronIdx = sparseConnections[i].first;
            neuronIdx = sparseConnections[i].second;
            neurons[neuronIdx]->setPresynapticLinks(preLayer.neurons[preNeuronIdx], preLayer.getNumNeurons());
        }
    } else {
        int preNumNeurons = preLayer.getNumNeurons();

        for (int i = 0; i < numNeurons; i++) {
            for (int j = 0; j < preNumNeurons; j++) {
                neurons[i]->setPresynapticLinks(preLayer.neurons[j], preLayer.getNumNeurons());
            }
        }
    }
}

void Layer::feedForward(const string& baseName, int classLabel, int t) {
    int spike = 0;

    if (type == "Output") {
        string filename = baseName + ".txt";
        ofstream file(filename, ios::app);

        for (int i = 0; i < numNeurons; i++) {
            spike = neurons[i]->updateNeuronState(t);
            if (spike == 1) file << t << ", " << i << ", " << classLabel << endl;
            // spikeHistory[i] += spike;
        }

        file.close();
    } else {
        for (int i = 0; i < numNeurons; i++) neurons[i]->updateNeuronState(t);
    }
}

void Layer::saveWeights(const string& baseName, const int layerId) {
    string fileName = baseName + to_string(layerId) + "_" + type + ".txt";

    if (filesystem::exists(fileName)) {
        filesystem::remove(fileName);
    }

    cout << "Saving weights to " << fileName << endl;

    for (int i = 0; i < numNeurons; i++) {
        neurons[i]->saveWeights(fileName, i);
    }
}

void Layer::loadWeights(const string& baseName, const int layerId) {
    string fileName = baseName + to_string(layerId) + "_" + type + ".txt";

    if (!filesystem::exists(fileName)) {
        cerr << "Error: Weights file " << fileName << " does not exist." << endl;
        return;
    }

    for (int i = 0; i < numNeurons; i++) {
        neurons[i]->loadWeights(fileName);
    }
}


// void Layer::showSpikeHistory() {
//     for (unsigned long i = 0; i < neurons.size(); i++) {
//         cout << "Neuron " << i << " Spike History: " << spikeHistory[i] << endl;
//     }
// }