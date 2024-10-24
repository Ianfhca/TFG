#include "../include/Layer.h"

Layer::Layer() {

};

// Layer::Layer(string type_, int numNeurons_, string connections_) : type(type_), numNeurons(numNeurons_), connections(connections_) {
//     for (int i = 0; i < numNeurons; i++) {
//         LIFneuron neuron;
//         neurons.push_back(neuron);
//         // membranePotentials.push_back(neuron.getMembranePotential());
//         // spiked.push_back(false);
//     }
// };

int Layer::initLayer(string type_, int numNeurons_, string connections_, vector<pair<int, int>> sparseConnections_) {
    type = type_;
    numNeurons = numNeurons_;
    connections = connections_;
    sparseConnections = sparseConnections_;
    for (int i = 0; i < numNeurons; i++) {
        LIFneuron neuron;
        neurons.push_back(neuron);
        // membranePotentials.push_back(neuron.getMembranePotential());
        // spiked.push_back(false);
    }
    return 0;
};

string Layer::getType() {
    return type;
};

int Layer::getNumNeurons() {
    return numNeurons;
};

string Layer::getConnections() {
    return connections;
};

vector<pair<int, int>> Layer::getSparseConnections() {
    return sparseConnections;
};

vector<LIFneuron> Layer::getNeurons() {
    return neurons;
};

void Layer::setPostSynapticLinks(Layer &postLayer) {
    if (connections.compare("sparse") == 0) {
        for (int i = 0; i < sparseConnections.size(); i++) {
            neurons[sparseConnections[i].first].setPostSynapticLink(postLayer.getNeurons()[sparseConnections[i].second]);
            // cout << sparseConnections[i].first << " -> " << sparseConnections[i].second << endl;
        }
    } else {
        for (int i = 0; i < neurons.size(); i++) {
            for (int j = 0; j < postLayer.getNeurons().size(); j++) {
                neurons[i].setPostSynapticLink(postLayer.getNeurons()[j]);
                // cout << i << " -> " << j << endl;
            }
        }
    }
}