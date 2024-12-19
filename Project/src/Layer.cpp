#include "../include/Layer.h"

// Layer::Layer(string type_, int numNeurons_, string connections_, int multisynapses_, vector<pair<int, int>> sparseConnections_)
//     : type(type_), numNeurons(numNeurons_), connections(connections_), multisynapses(multisynapses_), sparseConnections(sparseConnections_) {

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
            // cout << "Neuron " << i << " created. V=" << neuron.getMembranePotential() << endl;
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

vector<pair<int, int>> Layer::getSparseConnections() {
    return sparseConnections;
};

vector<LIFneuron> Layer::getNeurons() { // Check this getNeurons devuelve una copia del vector?? Problema. Tal vez deberia devolver un puntero
    return neurons;
};

// nl * nl-1 * multisynapses = numPostNeurons * numNeurons * multisynapses

// void Layer::setPostsynapticLinks(Layer &postLayer) {
//     // Layer *postLayerPtr = &postLayer;
    
//     if (connections.compare("sparse") == 0) {
//         for (int i = 0; i < sparseConnections.size(); i++) {
//             neurons[sparseConnections[i].first].setPostsynapticLink(postLayer.getNeurons()[sparseConnections[i].second]);
//             // neurons[sparseConnections[i].first].setPostSynapticLink(postLayerPtr->getNeurons()[sparseConnections[i].second]);
//         }
//     } else {
//         for (int i = 0; i < numNeurons; i++) {
//             for (int j = 0; j < postLayer.getNumNeurons(); j++) {
//                 neurons[i].setPostsynapticLink(postLayer.getNeurons()[j]);
//                 // neurons[i].setPostSynapticLink(postLayerPtr->getNeurons()[j]);
//             }
//         }
//     }
// }

void Layer::setPresynapticLinks(Layer &preLayer) {
    // Layer *preLayerPtr = &preLayer;

    if (connections.compare("sparse") == 0) {
        for (int i = 0; i < sparseConnections.size(); i++) {
            neurons[sparseConnections[i].first].setPresynapticLink(preLayer.getNeurons()[sparseConnections[i].second]); // Check this getNeurons devuelve una copia del vector?? Problema. Tal vez deberia devolver un puntero
            // neurons[sparseConnections[i].first].setPostSynapticLink(postLayerPtr->getNeurons()[sparseConnections[i].second]);
        }
    } else {
        for (int i = 0; i < numNeurons; i++) {
            for (int j = 0; j < preLayer.getNumNeurons(); j++) {
                neurons[i].setPresynapticLink(preLayer.getNeurons()[j]);
                // neurons[i].setPostSynapticLink(postLayerPtr->getNeurons()[j]);
            }
        }
    }
}

void Layer::feedForward(Layer &preLayer, double t) {
    double forcingFunction = 0.0;

    for (int i = 0; i < numNeurons; i++) {
        neurons[i].updatePresinapticTrace();
    }
}

    // neurons[j].setPresynapticTrace(d);
//     // preSynapticTrace[i][j][d] = (-preSynapticTrace[i][j][d] + (alpha * neurons[j].getSpike(d))) * (0.1 / 20.0); // (dt / lambdaX);
//     forcingFunction += neurons[j].updateForcingFunction(d);
//     neurons[j].setSpikeAtributes(tau[i][j][d], neurons[j].getPostsynapticSpike(), d);
//     // cout << tau[i][j][d] << endl;

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