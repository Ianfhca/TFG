#include "../include/Layer.h"

// Layer::Layer(string type_, int numNeurons_, string connections_, int multisynapses_, vector<pair<int, int>> sparseConnections_)
//     : type(type_), numNeurons(numNeurons_), connections(connections_), multisynapses(multisynapses_), sparseConnections(sparseConnections_) {

Layer::Layer(string type_, int numNeurons_, vector<pair<vector<double>, int>> neuronParams, string connections_, int multisynapses_, vector<pair<int, int>> sparseConnections_) {
    type = type_;
    numNeurons = numNeurons_;
    connections = connections_;
    multisynapses = multisynapses_;
    sparseConnections = sparseConnections_;

    for (int i = 0; i < neuronParams.size(); i++) {
        for (int j = 0; j < neuronParams[i].second; j++) {
            LIFneuron neuron(multisynapses, neuronParams[i].first[0], neuronParams[i].first[1], neuronParams[i].first[2], neuronParams[i].first[3], neuronParams[i].first[4], neuronParams[i].first[5], neuronParams[i].first[6], neuronParams[i].first[7]);
            neurons.push_back(neuron);
            // cout << "Neuron " << i << " created. V=" << neuron.getMembranePotential() << endl;
        }
    }
        
}

void Layer::initWeights(int numPostNeurons) {
    weights.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(multisynapses, 0.5)));

    // Visualize weights
    for (int k = 0; k < multisynapses; k++) {
        for (int i = 0; i < numPostNeurons; i++) {
            for (int j = 0; j < numNeurons; j++) {
                cout << weights[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

void Layer::initDelays(int numPostNeurons, double minT, double maxT) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(minT, maxT);
    tau.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(multisynapses)));

    for (int k = 0; k < multisynapses; k++) {
        for (int i = 0; i < numPostNeurons; i++) {
            for (int j = 0; j < numNeurons; j++) {
                tau[i][j][k] = roundTo(dis(gen), 0.1) * 10; 
                // tau[i][j][k] = roundTo(dis(gen), 0.1); 
                // tau[i][j][k] = dis(gen);
                cout << tau[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }    
}

void Layer::initPresynapticTrace(int numPostNeurons) {
    preSynapticTrace.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(multisynapses, 0.0)));
}

void Layer::setPostsynapticLinks(Layer &postLayer) {
    // Layer *postLayerPtr = &postLayer;
    
    if (connections.compare("sparse") == 0) {
        for (int i = 0; i < sparseConnections.size(); i++) {
            neurons[sparseConnections[i].first].setPostsynapticLink(postLayer.getNeurons()[sparseConnections[i].second]);
            // neurons[sparseConnections[i].first].setPostSynapticLink(postLayerPtr->getNeurons()[sparseConnections[i].second]);

            // cout << sparseConnections[i].first << " -> " << sparseConnections[i].second << endl;
        }
    } else {
        for (int i = 0; i < numNeurons; i++) {
            for (int j = 0; j < postLayer.getNumNeurons(); j++) {
                neurons[i].setPostsynapticLink(postLayer.getNeurons()[j]);
                // cout << "Link " << i  << " -> " << j << postLayer.getNeurons()[j].getMembranePotential() << endl;
                // LIFneuron *postNeuronPtr = neurons[i].getSynapses()[j].getPostNeuron()->getMembranePotential();
                // neurons[i].setPostSynapticLink(postLayerPtr->getNeurons()[j]);
                // cout << i << " -> " << j << endl;
            }
            
        }
    }
}

void Layer::feedForward(Layer &postLayer, double t, double alpha) {
    double forcingFunction = 0.0;

    for (int i = 0; i < postLayer.getNumNeurons(); i++) {
        for (int j = 0; j < numNeurons; j++) {
            // neuron[j]
            for (int d = 0; d < multisynapses; d++) {
                neurons[j].setPresynapticTrace(d);
                // preSynapticTrace[i][j][d] = (-preSynapticTrace[i][j][d] + (alpha * neurons[j].getSpike(d))) * (0.1 / 20.0); // (dt / lambdaX);
                forcingFunction += neurons[j].updateForcingFunction(d);
                neurons[j].setSpikeAtributes(tau[i][j][d], neurons[j].getPostsynapticSpike(), d);
                // cout << tau[i][j][d] << endl;
            }
        }
        postLayer.neurons[i].setPostsynapticSpike(postLayer.neurons[i].updateMembranePotential(forcingFunction, t));
        forcingFunction = 0.0;
        cout << "Neuron " << i << " v: " << postLayer.neurons[i].getMembranePotential() << " s(" << "Valor spike" << ") at time " << t << endl;
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

vector<LIFneuron> Layer::getNeurons() {
    return neurons;
};

// void Layer::updatePreSynapticTrace(Layer &preLayer, double t, double alpha) {

//     // int spikePos = 0;
    
//     for (int i = 0; i < preSynapticTrace.size(); i++) {
//         for (int j = 0; j < preSynapticTrace[i].size(); j++) {
//             for (int d = 0; d < preSynapticTrace[i][j].size(); d++) {    
//                 preSynapticTrace[i][j][d] = (-preSynapticTrace[i][j][d] + (alpha * neurons[j].getSpike(d))) * (0.1 / 20.0); // (dt / lambdaX);
//             }
//         }
//     }
// }

// void Layer::propagateSpikes(Layer &postLayer, double t) {

//     double forcingFunction = 0.0;
//     int spikePos = 0;
//     vector<int> spikeValue(weights.size(), 0);

//     for (int i = 0; i < weights.size(); i++) {
//         for (int j = 0; j < weights[i].size(); j++) {
//             for (int d = 0; d < weights[i][j].size(); d++) {
//                 forcingFunction += neurons[j].getSpike(d) * weights[i][j][d] - preSynapticTrace[i][j][d];
//                 neurons[j].setSpikeAtributes(tau[i][j][d] * 10, neurons[j].getPostsynapticSpike(), d);
//             }
//         }
//         neurons[i].setPostsynapticSpike(neurons[i].updateMembranePotential(forcingFunction, t));
        
//         // if (spikeValue == 1) {
//         //     neurons[i].modifySpikeAtributes(1, 1);
//         // }
//         // for (int d = 0; d < multisynapses; d++) {
//         //     neurons[i].modifySpikeAtributes(tau[][][] * 10, spikeValue, d);
//         // }
//         // neurons[i].modifySpikeAtributes(3, spikeValue, );
//         forcingFunction = 0.0;
        
//         cout << "Neuron " << i << " v: " << neurons[i].getMembranePotential() << " s(" << "Valor spike" << ") at time " << t << endl;
//     }
// } 