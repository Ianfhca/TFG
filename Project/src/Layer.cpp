// #include <opencv2/opencv.hpp>
#include "../include/Layer.hpp"


Layer::Layer(const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt)
    : type(topology.type), height(topology.height), width(topology.width), channels(topology.channels),
      numNeurons(topology.numNeurons), neuronType(topology.neuronType), connections(topology.connections),
      rDim(topology.r), multisynapses(topology.multisynapses), sparseConnections(topology.sparseConnections) {

    // spikeHistory.reserve(11);
    if (type == "Output") spikeHistory.resize(numNeurons, 0);

    int neuronIdx = 0;

    //print height, width, channels, numNeurons
    // cout << "Creating layer " << type << " with " << height << " height, " << width << " width, " << channels << endl;

    for (int ch = 0; ch < channels; ch++) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                neuronIdx = ((ch * height * width) + (i * width) + j);
                vector<int> neuronId = {i, j, ch, neuronIdx}; // Store neuron ID with additional info
                neurons.emplace_back(make_shared<LIFneuron>(neuronId, topology, neuronParams, dt));
                // neurons.emplace_back(make_shared<LIFneuron>(neuronIdx, topology, neuronParams, dt));
            }
        }
    }
}

// Layer::Layer(const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt) {
//     type = topology.type;
//     height = topology.height;
//     width = topology.width;
//     channels = topology.channels;
//     // numNeurons = topology.numNeurons;
//     numNeurons = topology.numNeurons;
//     neuronType = topology.neuronType;
//     connections = topology.connections;
//     multisynapses = topology.multisynapses;
//     sparseConnections = topology.sparseConnections;
//     rDim = topology.r;
//     // kDim = topology.k;

//     spikeHistory.reserve(11);
//     spikeHistory.resize(11, 0);

//     int neuronIdx = 0;

//     for (int ch = 0; ch < channels; ch++) {
//         for (int i = 0; i < height; i++) {
//             for (int j = 0; j < width; j++) {
//                 neuronIdx = ((ch * height * width) + (i * width) + j);
//                 neurons.emplace_back(make_shared<LIFneuron>(neuronIdx, topology, neuronParams, dt));
//                 // neurons.emplace_back(make_shared<LIFneuron>(neuronIdx, topology.neuronType, topology.multisynapses, topology.delayMin, topology.delayMax, neuronParams.vTh, neuronParams.vRest, neuronParams.vReset, neuronParams.lambdaV, neuronParams.tRefr, neuronParams.lambdaX, neuronParams.alpha, dt));
//             }
//         }
//     }
// }

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

// bool Layer::getWtaRule() {
//     return wtaRule;
// }

shared_ptr<LIFneuron> Layer::getNeuron(int i) {
    return neurons[i];
};

vector<pair<int, int>>& Layer::getSparseConnections() {
    return sparseConnections;
};

// void Layer::setPreWtaRule(bool preWtaRule_) {
//     preWtaRule = preWtaRule_;
// }

void Layer::setPresynapticLinks(Layer &preLayer) {
    // if (connections == "local") {
    //     int preHeight = preLayer.getHeight();
    //     int preWidth = preLayer.getWidth();
    //     int preChannels = preLayer.getChannels();

    //     int preY, preX, preCh = 0;
    //     int neuronIdx, preNeuronIdx = 0;

    //     for (int ch = 0; ch < channels; ch++) {
    //         for (int i = 0; i < height; i++) {
    //             for (int j = 0; j < width; j++) {
    //                 neuronIdx = ((ch * height * width) + (i * width) + j);
    //                 for (int y = 0; y < rDim; y++) {
    //                     for (int x = 0; x < rDim; x++) {
    //                         preY = i + y;
    //                         preX = j + x;
    //                         preCh = ch % preChannels;
    //                         if (preY >= 0 && preY < preHeight && preX >= 0 && preX < preWidth) {
    //                             preNeuronIdx = ((preCh * preHeight * preWidth) + (preY * preWidth) + preX);
    //                             // preNeuronIdx = preChannels * preHeight * preWidth + preY * preWidth + preX;
    //                             if (preNeuronIdx < preLayer.getNumNeurons()) {
    //                                 neurons[neuronIdx]->setPresynapticLink(preLayer.neurons[preNeuronIdx]);
    //                             }
    //                             // cout << "Neuron " << neuronIdx << " connected to Pre-Neuron " << preNeuronIdx  << " (H: " << i << ", W: " << j << ", ch: " << ch << ")" << " (H: " << preY << ", W: " << preX << ", Ch: " << preCh << ")" << endl;  
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //         // cout << "Neuron " << neuronIdx << " connected to Pre-Neuron " << preNeuronIdx  << " (ch: " << ch << ")" << " (H: " << preY << ", W: " << preX << ", Ch: " << preCh << ")" << endl;
    //     }
    
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
                            for (int x = 0; x < rDim; x++) {
                                // int preY = i + y - rDim / 2;
                                // int preX = j + x - rDim / 2;
                                int preY = i + y;
                                int preX = j + x;
                                if (preY >= 0 && preY < preHeight && preX >= 0 && preX < preWidth) {
                                    int preNeuronIdx = ((preCh * preHeight * preWidth) + (preY * preWidth) + preX);
                                    if (preNeuronIdx < preLayer.getNumNeurons()) {
                                        neurons[neuronIdx]->setPresynapticLink(preLayer.neurons[preNeuronIdx], preLayer.getNumNeurons());
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
            neurons[neuronIdx]->setPresynapticLink(preLayer.neurons[preNeuronIdx], preLayer.getNumNeurons());
            // cout << "Neuron " << neuronIdx << " connected to Pre-Neuron " << preNeuronIdx << endl;
        }
    } else {
        int preNumNeurons = preLayer.getNumNeurons();

        for (int i = 0; i < numNeurons; i++) {
            for (int j = 0; j < preNumNeurons; j++) {
                neurons[i]->setPresynapticLink(preLayer.neurons[j], preLayer.getNumNeurons()); // Check this getNeurons
            }
        }
    }
}

void Layer::feedForward(const string& baseName, int classLabel, int t) {
    int spike = 0;
    
    // bool wta = (connections == "local" || connections == "sparse") ? true : false;
    // cout << "Layer " << layerId << " number of neurons: " << numNeurons << endl;
    if (type == "Output") {
        string filename = baseName + ".txt";
        ofstream file(filename, ios::app);

        for (int i = 0; i < numNeurons; i++) {
            spike = neurons[i]->updateNeuronState(t);
            spikeHistory[i] += spike;
            if (spike == 1) file << t << ", " << i << ", " << classLabel << endl;
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
        // cout << "Loaded weights for neuron " << i << " from " << fileName << endl;
    }
}


void Layer::showSpikeHistory() {
    for (unsigned long i = 0; i < neurons.size(); i++) {
        cout << "Neuron " << i << " Spike History: " << spikeHistory[i] << endl;
    }
}