/**
 * @file LIFneuron.cpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Implements the LIFneuron behavior, including state updates, STDP learning and WTA inhibition.
 */

#include "../include/LIFneuron.hpp"

LIFneuron::LIFneuron(vector<int> neuronId_, const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt) 
    : neuronId(neuronId_), layer(topology.type), type(topology.neuronType), multisynapses(topology.multisynapses), delayMin(topology.delayMin), 
    delayMax(topology.delayMax), stdpRule(topology.stdpRule), wtaRule(topology.wtaRule), mode(topology.mode), v(neuronParams.vRest), vTh(neuronParams.vTh), 
    vRest(neuronParams.vRest), vReset(neuronParams.vReset), tauM(neuronParams.tauM), tRefr(neuronParams.tRefr), lambdaX(neuronParams.lambdaX), alpha(neuronParams.alpha), 
    learningRate(neuronParams.learningRate), aValue(neuronParams.aValue), convergenceTh(neuronParams.convergenceTh), dt(dt) {

    winit = 0;

    maxPreX = -DBL_MAX;
    minPreX = DBL_MAX;
    maxWeight = -DBL_MAX; 
    minWeight = DBL_MAX;

    tRefr = tRefr / dt;
    inRefraction = false;
    timeLastSpike = 0;
    vMax = 0;
    spike = 0;
    if (mode == "test") learning = false;
    else learning = true;
    postNeuronAmount = 0;
    remainingLinks = 0;
    wtaInhibition = false;
}

LIFneuron::~LIFneuron() {
    synapses.clear();
}

int LIFneuron::getNeuronId() {
    cout << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << ": ";
    return neuronId[3];
}

int LIFneuron::getType() {
    return type;
}

double LIFneuron::getMembranePotential() {
    return v;
}

double LIFneuron::getVMax() {
    return vMax;
}

int LIFneuron::getSpike() {
    return spike;
} 

void LIFneuron::setSpike(int spike_) {
    spike = spike_;
}

void LIFneuron::setPresynapticLinks(shared_ptr<LIFneuron> preNeuron, int numNeurons) { 
    int delay;

    for (int i = 0; i < multisynapses; i++) {
        delay = randomInt(delayMin, delayMax);
        winit = randomDouble(0.1, 0.5);
        synapses.emplace_back(make_shared<Synapse>(preNeuron, lambdaX, alpha, winit, delay, dt));
        preNeuron->addLink();
    }
}

void LIFneuron::addLink() {
    postNeuronAmount++;
    remainingLinks++;
}

int LIFneuron::getPostNeuronAmount() {
    return postNeuronAmount;
}

int LIFneuron::getRemainingLinks() {
    return remainingLinks;
}

void LIFneuron::setRemainingLinks(int remainingLinks_) { 
    remainingLinks = remainingLinks_; 
}

int LIFneuron::decrementRemainingLinks() { 
    if (remainingLinks > 0) remainingLinks--; 
    return remainingLinks; 
}

bool LIFneuron::getWtaInhibition() {
    return wtaInhibition;
}

void LIFneuron::setWtaInhibition(bool wtaInhibition_) {
    wtaInhibition = wtaInhibition;
}

void LIFneuron::inhibitNeuron(int t) {
    inRefraction = true;
    v = vReset;
    timeLastSpike = t;
    spike = 0;
}

int LIFneuron::updateNeuronState(int t) {
    int s = 0;
    double forcingFunction = 0.0;
    double aux = 0.0;
    shared_ptr<LIFneuron> preNeuron = nullptr;

    maxPreX = -DBL_MAX;
    minPreX = DBL_MAX;
    maxWeight = -DBL_MAX; 
    minWeight = DBL_MAX;

    long long winner = (wtaRule == 1) ? WTA() : -1; // Get the winner synapse index if WTA is enabled
    
    for (unsigned long i = 0; i < synapses.size(); i++) {
        if (synapses[i]) {
            preNeuron = synapses[i]->getPreNeuron();
            s = synapses[i]->obtainPreviousSpike();

            if (winner >= 0) { 
                preNeuron->setWtaInhibition(true);
                if ((unsigned long)winner == i && preNeuron->learning && layer != "Input") preNeuron->STDP();
                // else { if(layer != "Input") s = 0; } // If winner is not the current synapse and layer is not "Input", do not update the spike
            }
            
            synapses[i]->updatePresinapticTrace(s);
            forcingFunction += synapses[i]->updateForcingFunction(s);

            aux = synapses[i]->getPreSynapticTrace();
            if (aux > maxPreX) maxPreX = aux;
            if (aux < minPreX) minPreX = aux;
            
            aux = synapses[i]->getWeight();
            if (aux > maxWeight) maxWeight = aux;
            if (aux < minWeight) minWeight = aux;

            if (preNeuron->decrementRemainingLinks() == 0) {
                if (preNeuron->getWtaInhibition() == true) {
                    if (layer == "Input") preNeuron->spike = 0;
                    // else preNeuron->inhibitNeuron(t); // If the neuron is a winner, it does not inhibit itself
                    else if (preNeuron->inRefraction == false) preNeuron->inhibitNeuron(t);
                    preNeuron->setWtaInhibition(false);
                }                    
                preNeuron->setRemainingLinks(preNeuron->getPostNeuronAmount());
            }
        }
    }

    spike = updateMembranePotential(forcingFunction, t);

    if (learning && stdpRule == 1 && spike == 1) STDP(); // if there is a spike and learning and STDP rule are enabled, apply STDP

    return spike;
}

long long LIFneuron::WTA() {
 
    bool winnerFound = false;
    vector<long long> candidates;
    shared_ptr<LIFneuron> preNeuron = nullptr;

    double vMaxAux = -DBL_MAX;
    double currentMax = -DBL_MAX;
    
    if (!synapses.empty()) {
        for (unsigned long i = 0; i < synapses.size(); i++) {
            if (synapses[i]) {
                preNeuron = synapses[i]->getPreNeuron();
                vMaxAux = preNeuron->getVMax();

                if (preNeuron->getSpike() == 1 && vMaxAux >= currentMax) {
                    if (vMaxAux > currentMax) {
                        if (!candidates.empty()) {
                            candidates.clear();
                        }
                    }

                    winnerFound = true;
                    currentMax = vMaxAux;
                    candidates.push_back(i);
                }
            } else {
                cerr << "Error: synapse is null" << endl;
            }
        }
    }
    if (winnerFound && !candidates.empty()) return candidates[randomInt(0, candidates.size() - 1)];
    else return -1;
}

int LIFneuron::updateMembranePotential(double forcingFunction, int t) {
    int s = 0; // s(t) = 0

    if (inRefraction == true) {
        if ((t - timeLastSpike) > tRefr) inRefraction = false;
        else return s; // s(t) = 0
    }

    // double decay = exp(-dt / tauM); // Exponential decay (seems to be too fast)
    double decay = (1/tauM);
    v += decay * (-(v - vRest)) + forcingFunction;

    v = (v < vRest) ? vRest : v;

    if (v >= vTh) {
        s = 1; // s(t) = 1
        vMax = v;
        v = vReset;
        inRefraction = true;
        timeLastSpike = t;
    }

     return s;
}

void LIFneuron::STDP() {
    double LTP, LTD = 0.0;
    double weight = 0.0;
    double normPreX = 0.0;
    double normWeight = 0.0;
    double mse = 0.0;    

    for (unsigned long i = 0; i < synapses.size(); i++) {
        weight = synapses[i]->getWeight();
        normPreX = synapses[i]->getNormPreSynapticTrace(minPreX, maxPreX);
        LTP = exp(-weight + winit) * (exp(normPreX) - aValue); // LTPw * LTPx
        LTD = -exp(weight - winit) * (exp(1 - normPreX) - aValue); // LTDw * LTDx
        synapses[i]->updateWeight(learningRate * (LTP + LTD));

        normWeight = synapses[i]->getNormWeight(minWeight, maxWeight);
        mse += pow((normPreX - normWeight), 2);
    }
    
    mse = mse / synapses.size();
    MSE(mse);
}

void LIFneuron::MSE(double mse) {
    if (mse < convergenceTh) {
        learning = false;
        // cout << layer << " Neuron " << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " stopped learning with MSE: " << mse << endl; // Debugging line
    }
}

void LIFneuron::saveWeights(const string& fileName, int id) {
    ofstream file(fileName, ios::app);
    for (unsigned long i = 0; i < synapses.size(); i++) {
        file << id << ", " << i << ", " << synapses[i]->getWeight() << endl;
    }
    file.close();
}

void LIFneuron::loadWeights(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error opening file: " << fileName << endl;
        return;
    }

    string line;
    double weight;
    unsigned long index = 0;
    unsigned long id = 0;

    for (unsigned long i = 0; i < synapses.size(); i++) {
        getline(file, line);
        if (sscanf(line.c_str(), "%lu, %lu, %lf", &id, &index, &weight) == 3) {
            if (index < synapses.size()) {
                synapses[i]->setWeight(weight);
            } else {
                cerr << "Index out of bounds: " << index << endl;
            }
        }
    }

    file.close();
}