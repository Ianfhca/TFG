#include "../include/Layer.h"

Layer::Layer() {

};

int Layer::initLayer(string type_, int numNeurons_, string connections_, int multisynaptic_, vector<pair<int, int>> sparseConnections_) {
    type = type_;
    numNeurons = numNeurons_;
    connections = connections_;
    multisynaptic = multisynaptic_;
    sparseConnections = sparseConnections_;
    for (int i = 0; i < numNeurons; i++) {
        LIFneuron neuron;
        neurons.push_back(neuron);
    }

    // initWeights(numNeurons, numNeurons, multisynaptic);

    return 0;
};

void Layer::initWeights(int numNeurons, int numPostNeurons, int m) {
    cout << numPostNeurons << " " << numNeurons << " " << m << endl;
    weights.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(m, 0.5)));
    for (int k = 0; k < m; k++) {
        for (int i = 0; i < numPostNeurons; i++) {
            for (int j = 0; j < numNeurons; j++) {
                cout << weights[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
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

int Layer::getMultisynaptic() {
    return multisynaptic;
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