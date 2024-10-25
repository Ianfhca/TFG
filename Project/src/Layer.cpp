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

    return 0;
};

void Layer::initWeights(int numNeurons, int numPostNeurons, int m) {
    cout << numPostNeurons << " " << numNeurons << " " << m << endl;
    weights.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(m, 0.5)));

    // Visualize weights
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

void Layer::initDelays(int numNeurons, int numPostNeurons, int m, double minT, double maxT) {
    tau.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(m)));

    for (int k = 0; k < m; k++) {
        for (int i = 0; i < numPostNeurons; i++) {
            for (int j = 0; j < numNeurons; j++) {
                tau[i][j][k] = minT + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX/(maxT - minT)));
                cout << tau[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }    
}

void Layer::initPreSynapticTrace(int numNeurons, int numPostNeurons, int m) {
    preSynapticTrace.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(m, 0.0)));
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

void Layer::updatePreSynapticTrace(Layer &preLayer, double t, double alpha) {
    for (int i = 0; i < preLayer.preSynapticTrace.size(); i++) {
        for (int j = 0; j < preLayer.preSynapticTrace[i].size(); j++) {
            for (int d = 0; d < preLayer.preSynapticTrace[i][j].size(); d++) {
                preLayer.preSynapticTrace[i][j][d] = (-preLayer.preSynapticTrace[i][j][d] + (spiked[j] * (alpha - (alpha - preLayer.tau[i][j][d])))) * (0.1 / 20.0); // (dt / lambdaX);
                // if (preLayer.spiked[j]) {
                //    preLayer.preSynapticTrace[i][j][d] = (-preLayer.preSynapticTrace[i][j][d] + (alpha - (alpha - preLayer.tau[i][j][d]))) * (0.1 / 20.0);//(dt / lambdaX);
                //     // preSynapticTrace[i][j][d] = traces[i][j][d] * exp(-tauX) + alpha;
                // } else {
                //     preLayer.preSynapticTrace[i][j][d] = -preLayer.preSynapticTrace[i][j][d] * (0.1 / 20.0);//(dt / lambdaX);
                //     // preSynapticTrace[i][j][d] = traces[i][j][d] * exp(-tauX);
                // }
            }
        }
    }
}


void Layer::propagateSpikes(Layer &postLayer, double t) {

    double forcingFunction = 0.0;

    for (int i = 0; i < weights.size(); i++) {
        for (int j = 0; j < weights[i].size(); j++) {
            for (int d = 0; d < weights[i][j].size(); d++) {
                forcingFunction += spiked[j] * (weights[i][j][d] - (weights[i][j][d] -  tau[i][j][d])) - preSynapticTrace[i][j][d];
                // if (spiked[j]) {   
                //     forcingFunction = weights[i][j][d] * preSynapticTrace[i][j][d];
                // }
            }
        }
        postLayer.spiked[i] = postLayer.getNeurons()[i].updateMembranePotential(forcingFunction, t);
        forcingFunction = 0.0;
    }
}