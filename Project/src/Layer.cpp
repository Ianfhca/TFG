#include "../include/Layer.h"

// Layer::Layer(string type_, int numNeurons_, string connections_, int multisynaptic_, vector<pair<int, int>> sparseConnections_)
//     : type(type_), numNeurons(numNeurons_), connections(connections_), multisynaptic(multisynaptic_), sparseConnections(sparseConnections_) {

Layer::Layer(string type_, int numNeurons_, vector<pair<vector<double>, int>> neuronParams, string connections_, int multisynaptic_, vector<pair<int, int>> sparseConnections_) {
    type = type_;
    numNeurons = numNeurons_;
    connections = connections_;
    multisynaptic = multisynaptic_;
    sparseConnections = sparseConnections_;

    // Aclarar esto
    // Mirar si instanciar todas las neuronas for(neuronparams.size()) new LIFneuron
    // o instanciar una sola y hacer un push_back
    if (neuronParams.size() == 0) {
        LIFneuron neuron(multisynaptic);
        neurons.resize(numNeurons, neuron);
    } else {
        for (int i = 0; i < neuronParams.size(); i++) {
            LIFneuron neuron(neuronParams[i].second);
            neurons.push_back(neuron);
        }
    }
}

void Layer::initWeights(int numPostNeurons) {
    weights.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(multisynaptic, 0.5)));

    // Visualize weights
    for (int k = 0; k < multisynaptic; k++) {
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
    tau.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(multisynaptic)));

    for (int k = 0; k < multisynaptic; k++) {
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

void Layer::initPreSynapticTrace(int numPostNeurons) {
    preSynapticTrace.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(multisynaptic, 0.0)));
}

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

void Layer::feedForward(Layer &postLayer, double t, double alpha) {
    double forcingFunction = 0.0;

    for (int i = 0; i < preSynapticTrace.size(); i++) {
        for (int j = 0; j < preSynapticTrace[i].size(); j++) {
            for (int d = 0; d < preSynapticTrace[i][j].size(); d++) { 
                preSynapticTrace[i][j][d] = (-preSynapticTrace[i][j][d] + (alpha * neurons[j].getSpike(d))) * (0.1 / 20.0); // (dt / lambdaX);

                forcingFunction += neurons[j].getSpike(d) * weights[i][j][d] - preSynapticTrace[i][j][d];
                neurons[j].setSpikeAtributes(tau[i][j][d], neurons[j].getPostsynapticSpike(), d);
                // cout << tau[i][j][d] << endl;
            }
        }
        postLayer.neurons[i].setPostsynapticSpike(postLayer.neurons[i].updateMembranePotential(forcingFunction, t));
        forcingFunction = 0.0;
        cout << "Neuron " << i << " v: " << postLayer.neurons[i].getMembranePotential() << " s(" << "Valor spike" << ") at time " << t << endl;
    }
}

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
//         // for (int d = 0; d < multisynaptic; d++) {
//         //     neurons[i].modifySpikeAtributes(tau[][][] * 10, spikeValue, d);
//         // }
//         // neurons[i].modifySpikeAtributes(3, spikeValue, );
//         forcingFunction = 0.0;
        
//         cout << "Neuron " << i << " v: " << neurons[i].getMembranePotential() << " s(" << "Valor spike" << ") at time " << t << endl;
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

int Layer::getMultisynaptic() {
    return multisynaptic;
};

vector<pair<int, int>> Layer::getSparseConnections() {
    return sparseConnections;
};

vector<LIFneuron> Layer::getNeurons() {
    return neurons;
};