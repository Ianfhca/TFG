#include "../include/Layer.h"

Layer::Layer(string type_, int numNeurons_, vector<pair<vector<double>, int>> neuronParams, string connections_, int multisynapses_, pair<int, int> delayRange_, vector<pair<int, int>> sparseConnections_) {
    type = type_;
    numNeurons = numNeurons_;
    connections = connections_;
    multisynapses = multisynapses_;
    sparseConnections = sparseConnections_;

    for (int i = 0; i < neuronParams.size(); i++) {
        for (int j = 0; j < neuronParams[i].second; j++) {
            LIFneuron neuron(multisynapses, delayRange_, neuronParams[i].first[0], neuronParams[i].first[1], neuronParams[i].first[2], neuronParams[i].first[3], neuronParams[i].first[4], neuronParams[i].first[5], neuronParams[i].first[6], neuronParams[i].first[7]);
            neurons.push_back(neuron);
        }
    }
}

string Layer::getType() {
    return type;
};

int Layer::getNumNeurons() {
    return numNeurons;
};

string Layer::getConnections() {
    return connections;
};

int Layer::getMultisynapses() {
    return multisynapses;
};

vector<pair<int, int>>& Layer::getSparseConnections() {
    return sparseConnections;
};

vector<LIFneuron>& Layer::getNeurons() { // Check this
    return neurons;
};

void Layer::setPresynapticLinks(Layer &preLayer) {

    if (connections.compare("sparse") == 0) { // Check this sparse connections
        for (int i = 0; i < sparseConnections.size(); i++) {
            neurons[sparseConnections[i].first].setPresynapticLink(preLayer.getNeurons()[sparseConnections[i].second]);
        }
    } else {
        for (int i = 0; i < numNeurons; i++) {
            for (int j = 0; j < preLayer.getNumNeurons(); j++) {
                neurons[i].setPresynapticLink(preLayer.getNeurons()[j]); // Check this getNeurons
            }
        }
    }
}

void Layer::feedForward(int t) {
    int spike = 0;
    double forcingFunction = 0.0;

    for (int i = 0; i < numNeurons; i++) { // Check this optimize those functions in one
        neurons[i].updatePresinapticTrace();
        forcingFunction = neurons[i].updateForcingFunction();
        spike = neurons[i].updateMembranePotential(forcingFunction, t);
        neurons[i].updateSpikeAtributes();
    }
}

// void Layer::feedForward(Layer &postLayer, double t, double alpha) {
//     double forcingFunction = 0.0;

//     for (int i = 0; i < preSynapticTrace.size(); i++) {
//         for (int j = 0; j < preSynapticTrace[i].size(); j++) {
//             for (int d = 0; d < preSynapticTrace[i][j].size(); d++) { 
//                 preSynapticTrace[i][j][d] = (-preSynapticTrace[i][j][d] + (alpha * neurons[j].getSpike(d))) * (0.1 / 20.0); // (dt / lambdaX);

//                 forcingFunction += neurons[j].getSpike(d) * weights[i][j][d] - preSynapticTrace[i][j][d];
//                 neurons[j].setSpikeAtributes(tau[i][j][d], neurons[j].getPostsynapticSpike(), d);
//                 // cout << tau[i][j][d] << endl;
//             }
//         }
//         postLayer.neurons[i].setPostsynapticSpike(postLayer.neurons[i].updateMembranePotential(forcingFunction, t));
//         forcingFunction = 0.0;
//         cout << "Neuron " << i << " v: " << postLayer.neurons[i].getMembranePotential() << " s(" << "Valor spike" << ") at time " << t << endl;
//     }
// }