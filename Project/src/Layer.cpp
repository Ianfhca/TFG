#include "../include/Layer.h"

Layer::Layer(string type_, int numNeurons_, string connections_, int multisynaptic_, vector<pair<int, int>> sparseConnections_)
    : type(type_), numNeurons(numNeurons_), connections(connections_), multisynaptic(multisynaptic_), sparseConnections(sparseConnections_) {
    LIFneuron neuron();
    neurons.resize(numNeurons);
    int bufferSize = MAX_DELAY / MIN_DELAY;
    spiked.resize(numNeurons, vector<pair<int, double>>(bufferSize, make_pair(0, 0.0)));
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
};

void Layer::initDelays(int numPostNeurons, double minT, double maxT) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(minT, maxT);
    tau.resize(numPostNeurons, vector<vector<double>>(numNeurons, vector<double>(multisynaptic)));

    for (int k = 0; k < multisynaptic; k++) {
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
                spikePos = static_cast<int>(abs(preLayer.tau[i][j][d]) * 10) % 10; // Check this
                spikePos = preLayer.spiked[j].size() - 1 - spikePos;

                preLayer.preSynapticTrace[i][j][d] = (-preLayer.preSynapticTrace[i][j][d] + (alpha * preLayer.spiked[j][spikePos].first)) * (0.1 / 20.0); // (dt / lambdaX);
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
                spikePos = static_cast<int>(abs(tau[i][j][d]) * 10) % 10; // Check this
                spikePos = spiked[j].size() - 1 - spikePos;

                forcingFunction += spiked[j][spikePos].first * weights[i][j][d] - preSynapticTrace[i][j][d];
                // cout << "Spike Pos: " << spikePos << ", " << spiked[j][spikePos].first << ", " << spiked[j][spikePos].second << endl;
            }
        }
        postLayer.spiked[i].erase(postLayer.spiked[i].begin()); 
        spikeValue = postLayer.getNeurons()[i].updateMembranePotential(forcingFunction, t);
        postLayer.spiked[i].push_back(make_pair(spikeValue, t));
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