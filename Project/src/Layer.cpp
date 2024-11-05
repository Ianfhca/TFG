#include "../include/Layer.h"

Layer::Layer() {

};

int Layer::initLayer(string type_, int numNeurons_, string connections_, int multisynaptic_, vector<pair<int, int>> sparseConnections_) {
    type = type_;
    numNeurons = numNeurons_;
    connections = connections_;
    multisynaptic = multisynaptic_;
    sparseConnections = sparseConnections_;
    
    LIFneuron neuron;
    neurons.resize(numNeurons);
    int bufferSize = MAX_DELAY / MIN_DELAY;
    spiked.resize(numNeurons, vector<pair<int, double>>(bufferSize, make_pair(0, 0.0)));

    // for (int i = 0; i < numNeurons; i++) {
    //     cout << "Neuron " << i << " v: " << neurons[i].getMembranePotential();
    //     spiked[i][spiked[i].size() -1] = make_pair(rand() % 2, 0.1);
    //     for (int j = 0; j < bufferSize; j++) {
    //         // spiked[i][j].first = rand() % 2;
    //         cout << " s(" << spiked[i][j].first << "," << spiked[i][j].second <<  ") ";
    //     }
    //     cout << endl;
    // }

    return 0;
};

void Layer::initWeights(int numNeurons, int numPostNeurons, int m) {
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

double roundTo(double value, double precision) {
    return round(value / precision) * precision;
}

void Layer::initDelays(int numNeurons, int numPostNeurons, int m, double minT, double maxT) {
    tau.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(m)));
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(minT, maxT);


    for (int k = 0; k < m; k++) {
        for (int i = 0; i < numPostNeurons; i++) {
            for (int j = 0; j < numNeurons; j++) {
                // tau[i][j][k] = roundTo(dis(gen), 0.1);
                tau[i][j][k] = dis(gen);
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

    int spikePos = 0;
    
    for (int i = 0; i < preLayer.preSynapticTrace.size(); i++) {
        for (int j = 0; j < preLayer.preSynapticTrace[i].size(); j++) {
            for (int d = 0; d < preLayer.preSynapticTrace[i][j].size(); d++) {
                
                // for (int s = 0; s < preLayer.spiked[j].size(); s++) {
                //     if (t - preLayer.spiked[j][s].second >= preLayer.tau[i][j][d]) {
                //         spikePos = s;
                //     }
                // }
                spikePos = static_cast<int>(abs(preLayer.tau[i][j][d]) * 10) % 10;
                spikePos = preLayer.spiked[j].size() - 1 - spikePos;

                preLayer.preSynapticTrace[i][j][d] = (-preLayer.preSynapticTrace[i][j][d] + (alpha * preLayer.spiked[j][spikePos].first)) * (0.1 / 20.0); // (dt / lambdaX);
                // preLayer.preSynapticTrace[i][j][d] = (-preLayer.preSynapticTrace[i][j][d] + (spiked[j] * (alpha - (alpha - preLayer.tau[i][j][d])))) * (0.1 / 20.0); // (dt / lambdaX);
                // preLayer.preSynapticTrace[i][j][d] = (-preLayer.preSynapticTrace[i][j][d] + ((alpha * preLayer.spiked[j].first * t) - (alpha * preLayer.spiked[j].first * preLayer.tau[i][j][d]))) * (0.1 / 20.0); // (dt / lambdaX);
            }
        }
        
    }
}

void Layer::propagateSpikes(Layer &postLayer, double t) {

    double forcingFunction = 0.0;
    int spikePos = 0;
    int spikeValue = 0;

    for (int i = 0; i < weights.size(); i++) {
        for (int j = 0; j < weights[i].size(); j++) {
            for (int d = 0; d < weights[i][j].size(); d++) {
                // for (int s = 0; s < spiked[j].size(); s++) {
                //     if (t - spiked[j][s].second >= tau[i][j][d]) {
                //         spikePos = s;
                //     }
                // }

                spikePos = static_cast<int>(abs(tau[i][j][d]) * 10) % 10;
                spikePos = spiked[j].size() - 1 - spikePos;

                // cout << "Spike Pos: " << spikePos << ", " << spiked[j][spikePos].first << ", " << spiked[j][spikePos].second << endl;
                forcingFunction += spiked[j][spikePos].first * weights[i][j][d] - preSynapticTrace[i][j][d];
                // forcingFunction += spiked[j][spikePos].second * (weights[i][j][d] - (weights[i][j][d] -  tau[i][j][d])) - preSynapticTrace[i][j][d];
            }
        }
        postLayer.spiked[i].erase(postLayer.spiked[i].begin()); // Check this
        spikeValue = postLayer.getNeurons()[i].updateMembranePotential(forcingFunction, t);
        postLayer.spiked[i].push_back(make_pair(spikeValue, t));
        // postLayer.spiked[i].second = postLayer.getNeurons()[i].updateMembranePotential(forcingFunction, t);
        // cout << spiked[0][0].first << " Forcing Function: " << forcingFunction << endl;
        forcingFunction = 0.0;
        
        cout << "Neuron " << i << " v: " << postLayer.neurons[i].getMembranePotential() << " s(" << spikeValue << ") at time " << t << endl;
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

int Layer::getMultisynaptic() {
    return multisynaptic;
};

vector<pair<int, int>> Layer::getSparseConnections() {
    return sparseConnections;
};

vector<LIFneuron> Layer::getNeurons() {
    return neurons;
};