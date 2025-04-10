#include "../include/Layer.h"

Layer::Layer(const TopologyParameters &topology, const vector<pair<NeuronParameters, int>> &neuronParams, int dt) {
    type = topology.type;
    numNeurons = topology.numNeurons;
    connections = topology.connections;
    multisynapses = topology.multisynapses;
    sparseConnections = topology.sparseConnections;
    int neuronId = 0;

    for (int i = 0; i < neuronParams.size(); i++) {
        for (int j = 0; j < neuronParams[i].second; j++) {
            auto neuron = make_shared<LIFneuron>(neuronId, i, topology.multisynapses, topology.delayMin, topology.delayMax, neuronParams[i].first.vTh, neuronParams[i].first.vRest, neuronParams[i].first.vReset, neuronParams[i].first.lambdaV, neuronParams[i].first.tRefr, neuronParams[i].first.lambdaX, neuronParams[i].first.alpha, dt);
            neurons.push_back(neuron);
            neuronId++;
        }
    }
}

Layer::~Layer() {
    cout << "-----------------------" << endl;
    cout << "Destroying Layer " << type << endl;
    neurons.clear();
}

// Layer::Layer(const TopologyParameters &topology, const vector<pair<NeuronParameters, int>> &neuronParams, int dt) {
//     type = topology.type;
//     numNeurons = topology.numNeurons;
//     connections = topology.connections;
//     multisynapses = topology.multisynapses;
//     sparseConnections = topology.sparseConnections;
//     int neuronId = 0;

//     for (int i = 0; i < neuronParams.size(); i++) {
//         for (int j = 0; j < neuronParams[i].second; j++) {
//             auto neuron = make_shared<LIFneuron>(neuronId, i, topology.multisynapses, topology.delayMin, topology.delayMax, neuronParams[i].first.vTh, neuronParams[i].first.vRest, neuronParams[i].first.vReset, neuronParams[i].first.lambdaV, neuronParams[i].first.tRefr, neuronParams[i].first.lambdaX, neuronParams[i].first.alpha, dt);
//             // LIFneuron neuron(neuronId, i, topology.multisynapses, topology.delayMin, topology.delayMax, neuronParams[i].first.vTh, neuronParams[i].first.vRest, neuronParams[i].first.vReset, neuronParams[i].first.lambdaV, neuronParams[i].first.tRefr, neuronParams[i].first.lambdaX, neuronParams[i].first.alpha, dt);
//             // neurons.emplace_back(make_shared<LIFneuron>(neuronId, i, topology.multisynapses, topology.delayMin, topology.delayMax, neuronParams[i].first.vTh, neuronParams[i].first.vRest, neuronParams[i].first.vReset, neuronParams[i].first.lambdaV, neuronParams[i].first.tRefr, neuronParams[i].first.lambdaX, neuronParams[i].first.alpha, dt));
//             neurons.push_back(neuron);
//             neuronId++;
//         }
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

vector<pair<int, int>>& Layer::getSparseConnections() {
    return sparseConnections;
};

shared_ptr<LIFneuron> Layer::getNeuron(int i) {
    return neurons[i];
};

// vector<LIFneuron>& Layer::getNeurons() { // Check this
//     return neurons;
// };

void Layer::getNeuronsType() {
    for (int i = 0; i < neurons.size(); i++) {
        cout << "- Neuron " << i << " type: " << neurons[i]->getType() << endl;
    }
}

void Layer::setPresynapticLinks(Layer &preLayer) {
    if (connections == "sparse") { // Check this sparse connections
        for (const auto& connection : sparseConnections) {
            neurons[connection.first]->setPresynapticLink(preLayer.neurons[connection.second]); // Check this getNeurons
            // neurons[connection.first].setPresynapticLink(preLayer.neurons[connection.second]); // Check this getNeurons
        }
        // for (int i = 0; i < sparseConnections.size(); i++) {
        //     neurons[sparseConnections[i].first].setPresynapticLink(preLayer.neurons[sparseConnections[i].second]);
        // }
    } else {
        for (int i = 0; i < numNeurons; i++) {
            for (int j = 0; j < preLayer.getNumNeurons(); j++) {
                neurons[i]->setPresynapticLink(preLayer.neurons[j]); // Check this getNeurons
            }
        }
    }
}

void Layer::feedForward(int layerId, int t) {
    int spike = 0;
    
    for (int i = 0; i < numNeurons; i++) {
        // if (layerId == 0) {
        //     spike = neurons[i].updateMembranePotential(0.0, t);
        //     neurons[i].setSpike(spike);
        // }
        spike = neurons[i]->updateNeuronState(t);
    }
}