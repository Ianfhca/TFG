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

void LIFneuron::setPresynapticLink(shared_ptr<LIFneuron> preNeuron, int numNeurons) { 
    int delay;
    // double w;

    // static random_device rd;
    // static mt19937 generator(rd());

    // uniform_real_distribution<double> distribution(0.1, 0.5);
    // uniform_real_distribution<double> distribution(0.0, 1.0);
    // cout << delayMin << " " << delayMax << endl;

    for (int i = 0; i < multisynapses; i++) {
        // delay = randomNumber(delayMin, delayMax);
        delay = randomInt(delayMin, delayMax);
        // w = distribution(generator);
        // winit = w;
        winit = randomDouble(0.1, 0.5);
        // cout << winit << endl;
        synapses.emplace_back(make_shared<Synapse>(preNeuron, lambdaX, alpha, winit, delay, dt));
        preNeuron->addLink();
    }
}

// void LIFneuron::setPresynapticLink(shared_ptr<LIFneuron> preNeuron, int numNeurons) { 
//     int delay;
//     double w;

//     static random_device rd;
//     static mt19937 generator(rd());
    
//     // Kaiming He Initialization for ReLU use sqrt(2/fan_in)
//     // For other activation functions, use sqrt(1/fan_in) or sqrt(6/(fan_in + fan_out))
//     double std_dev = sqrt(1.0 / static_cast<double>(numNeurons));
//     normal_distribution<double> distribution(0.0, std_dev);
    
//     for (int i = 0; i < multisynapses; i++) {
//         delay = randomNumber(delayMin, delayMax);
//         w = distribution(generator);
//         w = max(-1.0, min(1.0, w)); // Clamp between [-1, 1]
//         winit = w;
//         synapses.emplace_back(make_shared<Synapse>(preNeuron, lambdaX, alpha, winit, delay, dt));
//         preNeuron->addLink();
//     }
// }

void LIFneuron::addLink() {
    postNeuronAmount++;
    remainingLinks++;
    // linkAmount++;
    // usageCount++;
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

    // spike = 0;
    maxPreX = -DBL_MAX;
    minPreX = DBL_MAX;
    maxWeight = -DBL_MAX; 
    minWeight = DBL_MAX;

    long long winner = (wtaRule == 1) ? WTA() : -1;

    // if (winner >= 0) {
    //     cout << "Winner: " << winner << endl;
    // } else {
    //     cout << "No winner found." << endl;
    // }
    
    for (unsigned long i = 0; i < synapses.size(); i++) {
        if (synapses[i]) {
            preNeuron = synapses[i]->getPreNeuron();
            // synapses[i]->updateSpikeAtributes();
            // s = synapses[i]->obtainSpike();
            s = synapses[i]->obtainPreviousSpike();
            // if (layer == "Output") cout << "Spike = " << s << endl;

            if (winner >= 0) { 
                preNeuron->setWtaInhibition(true);
                if (winner == i && preNeuron->learning && layer != "Input") preNeuron->STDP();
                else s = 0; // If winner is not the current synapse, do not update the spike
                // cout << "Prueba" << endl;
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
                    // else preNeuron->inhibitNeuron(t);
                    else if (preNeuron->inRefraction == false) preNeuron->inhibitNeuron(t);
                    preNeuron->setWtaInhibition(false);
                }                    
                preNeuron->setRemainingLinks(preNeuron->getPostNeuronAmount());
            }
        }
    }

    // if (maxPreX > 0 || minPreX > 0) cout << " " << maxPreX << " " << minPreX << " " << maxWeight << " " << minWeight << endl;
    // cout << forcingFunction << " " << v << " " << vMax << endl;
    spike = updateMembranePotential(forcingFunction, t);

    if (learning && stdpRule == 1 && spike == 1) {
        STDP();
    }

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
            if (synapses[i]) { // Delete this
                preNeuron = synapses[i]->getPreNeuron();
                vMaxAux = preNeuron->getVMax();
                // if (preNeuron->getLinkAmount() == preNeuron->getUsageCount()) preNeuron->setWtaInhibition(false);
                if (preNeuron->getSpike() == 1 && vMaxAux >= currentMax) {
                    if (vMaxAux > currentMax) {
                        if (!candidates.empty()) {
                            candidates.clear();
                        }
                        // cout << "[WTA] Candidate " << i << ", Vmax = " << preNeuron->getVMax() << ", Spike = " << preNeuron->getSpike() << endl;
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
    // spike = 0; // s(t) = 0
    if (inRefraction == true) {
        // if (t - timeLastSpike >= tRefr) inRefraction = false;
        // cout << layer << " Neuron " << getNeuronId() << " in refraction at t = " << t << timeLastSpike << tRefr << endl;
        if ((t - timeLastSpike) > tRefr) inRefraction = false; // Check this t-timeLastSpike >= tRefr
        else return s; // s(t) = 0
    }

    // v += (((-v) + vRest) + forcingFunction) * (dt / lambdaV);

    
    // if (v > -65) cout << layer << " Neuron " << getNeuronId() << " (v: " << v << " ff: " << forcingFunction << ")" << endl;
    // v += exp(-dt/lambdaV) * v + forcingFunction; // lambdaV = exp(-dt/tauM) && tauM = lambdaV.value(input parameter)
    // v += exp(-1/tauM) * v + forcingFunction; // JOSE OLD
    // v -= exp(-1/tauM) * v + forcingFunction; // JOSE BUENA

    // v = v + forcingFunction * (-exp(-1/tauM));
    // double decay = (1-exp(-1/tauM));
    double decay = (1/tauM);
    // v -= ((-v + vRest)) * decay + decay * forcingFunction;
    // v -= decay * (v + forcingFunction);
    // v = (exp(-1/tauM) * v) + forcingFunction;

    // if (v != vRest && layer == "Output") cout << layer << " Neuron " << getNeuronId() << " (v: " << v << " ff: " << forcingFunction << " decay: " << decay << " mp update: " << decay * (-(v - vRest)) + forcingFunction << ") " << endl;
    // v -= (-(v-vReset) + forcingFunction) * decay; // con pesos negativos y decay = 1/tauM
    v += decay * (-(v - vRest)) + forcingFunction; // con tau y lambda 100


    // double decay = exp(-dt / tauM);
    // v = decay * (v - vRest) + vRest + forcingFunction;
    // v -= (-(v - vRest) + forcingFunction) * exp(-1 / tauM);

    // if (v != vReset && layer != "Output") cout << layer << " Neuron " << getNeuronId() << " (v: " << v << " ff: " << forcingFunction << " decay: " << decay << " next v: " << (-(v-vReset) + forcingFunction) * decay << ") " << endl;
    v = (v < vRest) ? vRest : v;

    if (v >= vTh) {
        // if (layer != "Output") cout << layer << " Neuron " << getNeuronId() << " fired at iteration " << t << " | V = " << v << " f: " << forcingFunction << endl;
        // cout << layer << " Neuron " << getNeuronId() << " fired at iteration " << t << " (v: " << v << " ff: " <<  forcingFunction << " decay: " << decay <<") " << endl;
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
        // cout << layer << " WeightPre: " << weight << endl;
        normPreX = synapses[i]->getNormPreSynapticTrace(minPreX, maxPreX);
        // LTP = exp-(weight - winit) * exp(normPreX) - aValue; // LTPw *LTPx
        LTP = exp(-weight + winit) * (exp(normPreX) - aValue); // LTPw *LTPx
        LTD = -exp(weight - winit) * (exp(1 - normPreX) - aValue); // LTDw *LTDx
        synapses[i]->updateWeight(learningRate * (LTP + LTD));
        // weight = synapses[i]->getWeight();
        // cout << learningRate * (LTP + LTD) << endl;
        // cout << layer << " WeightPost: " << weight << endl;

        normWeight = synapses[i]->getNormWeight(minWeight, maxWeight);
        mse += pow((normPreX - normWeight), 2);
        // if (normPreX < 0 || normPreX > 1) cout << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " PreSynapticTrace: " << synapses[i]->getPreSynapticTrace() << endl;
        // if (normWeight < 0 || normWeight > 1) cout << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " NormWeight: " << normWeight << endl;
    }
    
    mse = mse / synapses.size();
    MSE(mse);
}

void LIFneuron::MSE(double mse) {
    // cout << "MSE: " << mse << endl;
    if (mse < convergenceTh) {
        learning = false;
        cout << layer << " Neuron " << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " stopped learning with MSE: " << mse << endl;
    }
}

// double LIFneuron::MSE(unsigned long index, double normPreX) {
//     double normWeight = 0.0;

//     normWeight = synapses[index]->getNormWeight(minWeight, maxWeight);
//     if (normWeight < 0 || normWeight > 1) cout << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " NormWeight: " << normWeight << endl;

//     return pow((normPreX - normWeight), 2);
// }

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



// if (winner >= 0) { // There is a winner synapse
    //     for (unsigned long i = 0; i < synapses.size(); i++) {
    //         if (synapses[i]) {
    //             preNeuron = synapses[i]->getPreNeuron();
    //             preNeuron->setHasBeenWinner(true);
    //             if (i == winner) {
    //                 synapses[winner]->updateSpikeAtributes();
    //                 s = synapses[winner]->obtainSpike();
    //                 synapses[winner]->updatePresinapticTrace(s);
    //                 forcingFunction += synapses[winner]->updateForcingFunction(s);
    //                 if (preNeuron->learning && layer != "Input") preNeuron->STDP();
    //             } else {
    //                 synapses[i]->updateSpikeAtributes();
    //                 s = synapses[i]->obtainSpike();
    //                 synapses[i]->updatePresinapticTrace(s);
    //                 forcingFunction += synapses[i]->updateForcingFunction(s);
    //             }

    //             aux = synapses[i]->getPreSynapticTrace();
    //             if (aux > maxPreX) maxPreX = aux;
    //             if (aux < minPreX) minPreX = aux;
    //             aux = synapses[i]->getWeight();
    //             if (aux > maxWeight) maxWeight = aux;
    //             if (aux < minWeight) minWeight = aux;

    //             if (preNeuron->decrementUsageCount() == 0 && preNeuron->getHasBeenWinner() == true) {
    //                 if (preNeuron->inRefraction == false) preNeuron->inhibitNeuron(t);
    //                 preNeuron->setHasBeenWinner(false);
    //                 preNeuron->setUsageCount(preNeuron->getLinkAmount());
    //             }

    //         } else {
    //             cerr << "Error: synapse is null" << endl;
    //         }
    //     }
    // } else {
    //     for (unsigned long i = 0; i < synapses.size(); i++) {
    //         if (synapses[i]) {
    //             synapses[i]->updateSpikeAtributes();
    //             s = synapses[i]->obtainSpike();
    //             synapses[i]->updatePresinapticTrace(s);
    //             forcingFunction += synapses[i]->updateForcingFunction(s);

    //             aux = synapses[i]->getPreSynapticTrace();
    //             if (aux > maxPreX) maxPreX = aux;
    //             if (aux < minPreX) minPreX = aux;
    //             aux = synapses[i]->getWeight();
    //             if (aux > maxWeight) maxWeight = aux;
    //             if (aux < minWeight) minWeight = aux;

    //             if (wtaRule == 1) {
    //                 preNeuron = synapses[i]->getPreNeuron();
    //                 if (preNeuron->decrementUsageCount() == 0 && preNeuron->getHasBeenWinner() == true) {
    //                     if (preNeuron->inRefraction == false) preNeuron->inhibitNeuron(t);
    //                     preNeuron->setHasBeenWinner(false);
    //                     preNeuron->setUsageCount(preNeuron->getLinkAmount());
    //                 }
    //             }
    //         } else {
    //             cerr << "Error: synapse is null" << endl;
    //         }
    //     }
    // }