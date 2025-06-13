#include "../include/LIFneuron.hpp"

LIFneuron::LIFneuron(vector<int> neuronId_, const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt) 
    : neuronId(neuronId_), layer(topology.type), type(topology.neuronType), multisynapses(topology.multisynapses), delayMin(topology.delayMin), 
    delayMax(topology.delayMax), stdpRule(topology.stdpRule), wtaRule(topology.wtaRule), mode(topology.mode), v(neuronParams.vRest), vTh(neuronParams.vTh), vRest(neuronParams.vRest), vReset(neuronParams.vReset), 
    tauM(neuronParams.tauM), tRefr(neuronParams.tRefr), lambdaX(neuronParams.lambdaX), alpha(neuronParams.alpha), winit(neuronParams.weight), 
    learningRate(neuronParams.learningRate), aValue(neuronParams.aValue), convergenceTh(neuronParams.convergenceTh), dt(dt) {

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
    linkAmount = 0;
    usageCount = 0;
    hasBeenWinner = false;
}

// LIFneuron::LIFneuron(int neuronId_, int type_, int multisynapses_, int delayMin_, int delayMax_, double vTh_, double vRest_, double vReset_, double lambdaV_, int tRefr_, double lambdaX_, double alpha_, int dt_) {
//     neuronId = neuronId_;
//     type = type_;
//     multisynapses = multisynapses_;
//     delayMin = delayMin_;
//     delayMax = delayMax_;
//     v = vRest_;
//     vTh = vTh_;
//     vRest = vRest_;
//     vReset = vReset_;
//     lambdaV = lambdaV_;
//     // tRefr = tRefr_;
//     tRefr = tRefr_ / dt_;
//     lambdaX = lambdaX_;
//     alpha = alpha_;
//     dt = dt_;

//     maxPreX = -DBL_MAX;
//     minPreX = DBL_MAX;
//     maxWeight = -DBL_MAX; 
//     minWeight = DBL_MAX;

//     inRefraction = false;
//     timeLastSpike = 0;
//     vMax = 0;

//     spike = 0;
//     learning = true;
// }

LIFneuron::~LIFneuron() {
    synapses.clear();
}

int LIFneuron::getNeuronId() {
    cout << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " / ";
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

// void LIFneuron::setPresynapticLink(shared_ptr<LIFneuron> preNeuron, int numNeurons) { 
//     int delay;
//     std::default_random_engine generator;
//     std::normal_distribution<float> distribution(0.0, std::sqrt(2.0f / numNeurons));
//     double w = 0.0;

//     for (int i = 0; i < multisynapses; i++) {
//         delay = randomNumber(delayMin, delayMax);
//         w = distribution(generator);
//         synapses.emplace_back(make_shared<Synapse>(preNeuron, lambdaX, alpha, w, delay, dt));
//         preNeuron->addLink();
//     }
// }

void LIFneuron::setPresynapticLink(shared_ptr<LIFneuron> preNeuron, int numNeurons) { 
    int delay;
    double w;
    
    // Inicializar el generador con semilla aleatoria
    static std::random_device rd;
    static std::mt19937 generator(rd());
    
    // Kaiming He Initialization para ReLU (usa sqrt(2/fan_in))
    // Para otras funciones de activación, usar sqrt(1/fan_in) o sqrt(6/(fan_in + fan_out))
    double std_dev = std::sqrt(1.0 / static_cast<double>(numNeurons));
    std::normal_distribution<double> distribution(0.0, std_dev);
    
    for (int i = 0; i < multisynapses; i++) {
        delay = randomNumber(delayMin, delayMax);
        w = distribution(generator);
        w = std::max(-1.0, std::min(1.0, w)); // Clamp between [-1, 1]
        winit = w;
        synapses.emplace_back(make_shared<Synapse>(preNeuron, lambdaX, alpha, winit, delay, dt));
        preNeuron->addLink();
    }
}

void LIFneuron::inhibitNeuron(int t) {
    inRefraction = true;
    v = vReset;
    timeLastSpike = t;
    spike = 0;
}

int LIFneuron::updateNeuronState(int t) {
    // double forcingFunction = WTA(t);
    // if (layer == "Merge") cout << wtaRule << endl;
    int s = 0;
    double forcingFunction = 0.0;
    double aux;
    shared_ptr<LIFneuron> preNeuron = nullptr;
    maxPreX = -DBL_MAX;
    minPreX = DBL_MAX;
    maxWeight = -DBL_MAX; 
    minWeight = DBL_MAX;

    // long long winner = (learning && wtaRule == 1) ? WTA(t) : -1;
    unsigned long winner = (wtaRule == 1) ? WTA() : -1;

    if (winner == -1) {
        // cout << "[WTA] No winner at t = " << t << " in neuron " << neuronId[0] << "," << neuronId[1] << "," << neuronId[2] << endl;
    } else {
        // cout << layer << " [WTA] Winner synapse at t = " << t << " in neuron " << neuronId[0] << "," << neuronId[1] << "," << neuronId[2] << " with index: " << winner << endl;
    }

    if (winner >= 0) { // There is a winner synapse
        for (unsigned long i = 0; i < synapses.size(); i++) {
            if (synapses[i]) {
                preNeuron = synapses[i]->getPreNeuron();
                preNeuron->setHasBeenWinner(true);
                if (i == winner) {
                    synapses[winner]->updateSpikeAtributes();
                    s = synapses[winner]->obtainSpike();
                    synapses[winner]->updatePresinapticTrace(s);
                    forcingFunction += synapses[winner]->updateForcingFunction(s);
                    if (preNeuron->learning && layer != "Input") preNeuron->STDP();
                } else {
                    // synapses[i]->updateSpikeAtributes();
                    s = synapses[i]->obtainSpike();
                    synapses[i]->updatePresinapticTrace(0);
                    forcingFunction += synapses[i]->updateForcingFunction(0);
                }

                aux = synapses[i]->getPreSynapticTrace();
                if (aux > maxPreX) maxPreX = aux;
                if (aux < minPreX) minPreX = aux;
                aux = synapses[i]->getWeight();
                if (aux > maxWeight) maxWeight = aux;
                if (aux < minWeight) minWeight = aux;

                if (preNeuron->decrementUsageCount() == 0 && preNeuron->getHasBeenWinner() == true) {
                    if (preNeuron->inRefraction == false) preNeuron->inhibitNeuron(t);
                    preNeuron->setHasBeenWinner(false);
                    preNeuron->setUsageCount(preNeuron->getLinkAmount());
                }

            } else {
                cerr << "Error: synapse is null" << endl;
            }
        }
    } else {
        for (unsigned long i = 0; i < synapses.size(); i++) {
            if (synapses[i]) {
                synapses[i]->updateSpikeAtributes();
                s = synapses[i]->obtainSpike();
                synapses[i]->updatePresinapticTrace(s);
                forcingFunction += synapses[i]->updateForcingFunction(s);

                aux = synapses[i]->getPreSynapticTrace();
                if (aux > maxPreX) maxPreX = aux;
                if (aux < minPreX) minPreX = aux;
                aux = synapses[i]->getWeight();
                if (aux > maxWeight) maxWeight = aux;
                if (aux < minWeight) minWeight = aux;

                if (wtaRule == 1) {
                    preNeuron = synapses[i]->getPreNeuron();
                    if (preNeuron->decrementUsageCount() == 0 && preNeuron->getHasBeenWinner() == true) {
                        if (preNeuron->inRefraction == false) preNeuron->inhibitNeuron(t);
                        preNeuron->setHasBeenWinner(false);
                        preNeuron->setUsageCount(preNeuron->getLinkAmount());
                    }
                }
            } else {
                cerr << "Error: synapse is null" << endl;
            }
        }
    }

    // cout << maxPreX << " " << minPreX << " " << maxWeight << " " << minWeight << endl;
    // if (layer == "SS-Conv") cout << maxPreX << " " << minPreX << " " << maxWeight << " " << minWeight << " " << forcingFunction << endl;
    // cout << forcingFunction << " " << v << " " << vMax << endl;
    updateMembranePotential(forcingFunction, t);
    // if (spike == 1) cout << "llega aqui" << endl;

    if (learning && stdpRule == 1 && spike == 1) {
        STDP();
        // cout << "[STDP] Neuron " << neuronId[0] << "," << neuronId[1] << "," << neuronId[2] << " at t = " << t << " updated weights." << endl;
    }

    return spike;
}

long long LIFneuron::WTA() {
    shared_ptr<LIFneuron> preNeuron = nullptr;

    // long long winner = -1;
    bool winnerFound = false;
    double vMaxAux = -DBL_MAX;
    vector<long long> candidates;
    double currentMax = -DBL_MAX;;
    
    if (!synapses.empty()) {
        for (unsigned long i = 0; i < synapses.size(); i++) {
            if (synapses[i]) { // Delete this
                preNeuron = synapses[i]->getPreNeuron();
                vMaxAux = preNeuron->getVMax();
                // if (preNeuron->getLinkAmount() == preNeuron->getUsageCount()) preNeuron->setHasBeenWinner(false);
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
    if (winnerFound && !candidates.empty()) return candidates[randomNumber(0, candidates.size() - 1)];
    else return -1;
}

int LIFneuron::updateMembranePotential(double forcingFunction, int t) { // Check this make private
    // int s = 0; // s(t) = 0
    spike = 0; // s(t) = 0
    if (inRefraction == true) {
        // if (t - timeLastSpike >= tRefr) inRefraction = false;
        // cout << layer << " Neuron " << getNeuronId() << " in refraction at t = " << t << timeLastSpike << tRefr << endl;
        if ((t - timeLastSpike) > tRefr) inRefraction = false; // Check this t-timeLastSpike >= tRefr
        else return spike; // s(t) = 0
    }

    // v += (((-v) + vRest) + forcingFunction) * (dt / lambdaV);

    // v += exp(-dt/lambdaV) * v + forcingFunction; // lambdaV = exp(-dt/tauM) && tauM = lambdaV.value(input parameter)
    // v += (-1/tauM) * v + forcingFunction; // JOSE
    v += exp(-dt/tauM) * v + forcingFunction; // JOSE

    // double decay = exp(-dt / tauM);
    // v = decay * (v - vRest) + vRest + forcingFunction;

    // v += (-(v - vRest) + forcingFunction) * exp(-dt / tauM);

    // cout << " Neuron " << getNeuronId() << " (v: " << v << " ff: " << forcingFunction << ")" << endl;
    v = (v < vRest) ? vRest : v;

    if (v >= vTh) {
        // cout << layer << " Neuron " << getNeuronId() << " fired at iteration " << t << " | V = " << v << " f: " << forcingFunction << endl;
        spike = 1; // s(t) = 1
        vMax = v;
        v = vReset;
        inRefraction = true;
        timeLastSpike = t;
    }

     return spike;
}

void LIFneuron::STDP() {
    double LTP, LTD = 0.0;
    double weight, normPreX = 0.0;
    double mse = 0.0;
    // double a = 0.0; // Check this use as parameter
    // double learningRate = 0.1; // Check this use as parameter
    // double lTh = 0.05; // Check this use as parameter
    

    for (unsigned long i = 0; i < synapses.size(); i++) {
        // winit = synapses[i]->getWinit();
        weight = synapses[i]->getWeight();
        normPreX = synapses[i]->getNormPreSynapticTrace(minPreX, maxPreX);
        LTP = exp(-weight + winit) * exp(normPreX) - aValue; // LTPw *LTPx
        LTD = -exp(weight - winit) * exp(1 - normPreX) - aValue; // LTDw *LTDx
        synapses[i]->updateWeight(learningRate * (LTP + LTD));
        mse += MSE(i, normPreX);
        // if (normPreX < 0 || normPreX > 1) cout << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " PreSynapticTrace: " << synapses[i]->getPreSynapticTrace() << endl;
    }
    
    mse /= synapses.size();
    // cout << mse << endl;

    // if (learning && mse > convergenceTh) {
    if (learning && mse < convergenceTh) {
        learning = false;
        cout << layer << " Neuron " << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " stopped learning with MSE: " << mse << endl;
        // cout << synapses.size() << " synapses." << endl;
    }
}

double LIFneuron::MSE(unsigned long index, double normPreX) {
    double normWeight = 0.0;

    normWeight = synapses[index]->getNormWeight(minWeight, maxWeight);
    // if (normWeight < 0 || normWeight > 1) cout << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " NormWeight: " << normWeight << endl;

    return pow((normPreX - normWeight), 2);
}

void LIFneuron::saveWeights(const string& fileName, int id) {
    ofstream file(fileName, ios::app);
    for (unsigned long i = 0; i < synapses.size(); i++) {
        file << id << ", " << i << ", " << synapses[i]->getWeight() << endl;
        // file << "W" << i << " = " << synapses[i]->getWeight() << endl;
        // cout << "W" << i << " = " << synapses[i]->getWeight() << endl; // Debugging output
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
        // if (sscanf(line.c_str(), "W%lu = %lf", &index, &weight) == 2) {
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

// void LIFneuron::STDP(unsigned long index) {
//     double LTP, LTD = 0.0;
//     double winit, weight, normPreX = 0.0;
//     double a = 0.0; // Check this use as parameter
//     double learningRate = 0.1; // Check this use as parameter

//     winit = synapses[index]->getWinit();
//     weight = synapses[index]->getWeight();
//     normPreX = synapses[index]->getNormPreSynapticTrace(minPreX, maxPreX);
//     LTP = exp(-weight + winit) * exp(normPreX) - a; // LTPw *LTPx
//     LTD = -exp(weight - winit) * exp(1 - normPreX) - a; // LTDw *LTDx
//     synapses[index]->setWeight(learningRate * (LTP + LTD));
// }

// double LIFneuron::MSE() { 
//     double mse = 0.0;
//     double normWeight = 0.0;
//     double normPreX = 0.0;

//     for (int i = 0; i < synapses.size(); i++) {
//         normWeight = synapses[i]->getNormWeight(minWeight, maxWeight);
//         normPreX = synapses[i]->getNormPreSynapticTrace(minPreX, maxPreX);

//         mse += pow(normPreX - normWeight, 2);
//     }

//     mse /= synapses.size();

//     return mse;
// }

// void LIFneuron::getFirstWeight() {
//     if (!synapses.empty()) {
//         for (unsigned long i = 0; i < synapses.size() / 2; i++) {
//             if (synapses[i]) {
//                 cout << "Neuron " << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << " synapse " << i << " weight: " << synapses[i]->getWeight() << endl;
//             }
//         }
//         // return synapses[0]->getWeight();
//     }
//     // return 0.0; 
// }

// int LIFneuron::gatherSpike(int t) {
//     int s = 0;
//     shared_ptr<LIFneuron> preNeuron = nullptr;

//     for (unsigned long i = 0; i < synapses.size(); i++) {
//         synapses[i]->updateSpikeAtributes();
//         s = synapses[i]->obtainSpike();
        
//         if (s == 1) {
//             preNeuron = synapses[i]->getPreNeuron();
//             preNeuron->STDP();
//         }
//     }

//     return s;
// }

void LIFneuron::addLink() {
    linkAmount++;
    usageCount++;
}

int LIFneuron::getLinkAmount() {
    return linkAmount;
}

int LIFneuron::getUsageCount() {
    return usageCount;
}

void LIFneuron::setUsageCount(int usageCount_) { 
    usageCount = usageCount_; 
}

int LIFneuron::decrementUsageCount() { 
    if (usageCount > 0) usageCount--; 
    return usageCount; 
}

bool LIFneuron::getHasBeenWinner() {
    return hasBeenWinner;
}

void LIFneuron::setHasBeenWinner(bool hasBeenWinner_) {
    hasBeenWinner = hasBeenWinner_;
}




// int LIFneuron::updateNeuronState(bool doWta, int t) {
//     double forcingFunction = WTA(t);

//     updateMembranePotential(forcingFunction, t);

//     if (wtaRule == false && spike == 1) STDP();

//     return spike;
// }

// double LIFneuron::WTA(int t) {
//     int s = 0;
//     double forcingFunction = 0.0;
//     double aux;
//     shared_ptr<LIFneuron> preNeuron = nullptr;

//     maxPreX = -DBL_MAX;
//     minPreX = DBL_MAX;
//     maxWeight = -DBL_MAX; 
//     minWeight = DBL_MAX;

//     unsigned long winner = 0;
//     bool winnerFound = false;
//     double vMaxAux = -DBL_MAX;
    
//     if (!synapses.empty()) {
//         if (learning) {
//             for (unsigned long i = 0; i < synapses.size(); i++) {
//                 if (synapses[i]) { // Delete this
//                     preNeuron = synapses[i]->getPreNeuron();
//                     if (preNeuron->getLinkAmount() == preNeuron->getUsageCount()) preNeuron->setHasBeenWinner(false);
//                     if (preNeuron->getSpike() == 1 && preNeuron->getVMax() > vMaxAux) {
//                         winnerFound = true;
//                         winner = i;
//                         vMaxAux = preNeuron->getVMax();
//                     }
//                 } else {
//                     cerr << "Error: synapse is null" << endl;
//                 }
//             }
//         }

//         if (winnerFound) { // There is a winner synapse
//             for (unsigned long i = 0; i < synapses.size(); i++) {
//                 if (synapses[i]) {
//                     preNeuron = synapses[i]->getPreNeuron();
//                     preNeuron->setHasBeenWinner(true);
//                     if (i == winner) {
//                         synapses[winner]->updateSpikeAtributes();
//                         s = synapses[winner]->obtainSpike();
//                         synapses[winner]->updatePresinapticTrace(s);
//                         forcingFunction += synapses[winner]->updateForcingFunction(s);
//                         preNeuron->STDP();
//                         // if (layer == "SS-Conv") cout << synapses[winner]->getPreNeuron()->getLinkAmount() << endl;
//                         // if (layer == "Output") cout << layer << " " << preNeuron->getNeuronId() << " winner synapse: " << neuronId[0] << " x " <<neuronId[1] << " x " << neuronId[2] << endl;
//                     } else {
//                         // synapses[winner]->updateSpikeAtributes();
//                         s = synapses[i]->obtainSpike();
//                         synapses[i]->updatePresinapticTrace(s);
//                         forcingFunction += synapses[i]->updateForcingFunction(s);
//                     }

//                     aux = synapses[i]->getPreSynapticTrace();
//                     if (aux > maxPreX) maxPreX = aux;
//                     if (aux < minPreX) minPreX = aux;
//                     aux = synapses[i]->getWeight();
//                     if (aux > maxWeight) maxWeight = aux;
//                     if (aux < minWeight) minWeight = aux;

//                     if (preNeuron->decrementUsageCount() <= 0 && preNeuron->getHasBeenWinner() == true) {
//                         preNeuron->inhibitNeuron(t);
//                         // preNeuron->setHasBeenWinner(false);
//                         preNeuron->setUsageCount(preNeuron->getLinkAmount());
//                     }

//                 } else {
//                     cerr << "Error: synapse is null" << endl;
//                 }
//             }
//         } else {
//             for (unsigned long i = 0; i < synapses.size(); i++) {
//                 if (synapses[i]) {
//                     synapses[i]->updateSpikeAtributes();
//                     s = synapses[i]->obtainSpike();
//                     synapses[i]->updatePresinapticTrace(s);
//                     forcingFunction += synapses[i]->updateForcingFunction(s);

//                     aux = synapses[i]->getPreSynapticTrace();
//                     if (aux > maxPreX) maxPreX = aux;
//                     if (aux < minPreX) minPreX = aux;
//                     aux = synapses[i]->getWeight();
//                     if (aux > maxWeight) maxWeight = aux;
//                     if (aux < minWeight) minWeight = aux;
//                 } else {
//                     cerr << "Error: synapse is null" << endl;
//                 }
//             }
//         }
//     }
//     return forcingFunction;
// }