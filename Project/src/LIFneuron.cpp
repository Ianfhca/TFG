#include "../include/LIFneuron.hpp"

// LIFneuron::LIFneuron(int multisynapses_, pair<int, int> delayRange_, double vTh_, double vRest_, double vReset_, double lambdaV_, double tRefr_, int dt_, double lambdaX_, double alpha_) {
//     v = vRest_;
//     vTh = vTh_;
//     vRest = vRest_;
//     vReset = vReset_;
//     lambdaV = lambdaV_;
//     tRefr = static_cast<int>(tRefr_);
//     dt = dt_;
//     lambdaX = lambdaX_;
//     alpha = alpha_;
//     multisynapses = multisynapses_;
//     delayRange = delayRange_;

//     inRefraction = false;
//     timeLastSpike = 0;

//     spike = 0;
// }
// LIFneuron::LIFneuron(int neuronId_, int type_, int multisynapses_, int delayMin_, int delayMax_, double vTh_, double vRest_, double vReset_, double lambdaV_, int tRefr_, double lambdaX_, double alpha_, int dt_)
//     : neuronId(neuronId_), type(type_), multisynapses(multisynapses_), delayMin(delayMin_), delayMax(delayMax_), v(vRest_), vTh(vTh_), vRest(vRest_), vReset(vReset_), lambdaV(lambdaV_), tRefr(tRefr_ / dt_), lambdaX(lambdaX_), alpha(alpha_), dt(dt_) {
//     // maxPreX = __DBL_MIN__;
//     // minPreX = __DBL_MAX__;
//     // maxWeight = __DBL_MIN__;
//     // minWeight = __DBL_MAX__;
//     inRefraction = false;
//     timeLastSpike = 0;
//     spike = 0;
// }

// Good one
LIFneuron::LIFneuron(int neuronId_, int type_, int multisynapses_, int delayMin_, int delayMax_, double vTh_, double vRest_, double vReset_, double lambdaV_, int tRefr_, double lambdaX_, double alpha_, int dt_) {
    neuronId = neuronId_;
    type = type_;
    multisynapses = multisynapses_;
    delayMin = delayMin_;
    delayMax = delayMax_;
    v = vRest_;
    vTh = vTh_;
    vRest = vRest_;
    vReset = vReset_;
    lambdaV = lambdaV_;
    // tRefr = tRefr_;
    tRefr = tRefr_ / dt_;
    lambdaX = lambdaX_;
    alpha = alpha_;
    dt = dt_;

    maxPreX = -DBL_MAX;
    minPreX = DBL_MAX;
    maxWeight = -DBL_MAX; 
    minWeight = DBL_MAX;

    inRefraction = false;
    timeLastSpike = 0;
    vMax = 0;

    spike = 0;
    learning = true;
}

LIFneuron::~LIFneuron() {
    // cout << "Destroying LIFneuron with ID: " << neuronId << endl;
    // cout << "Number of synapses: " << synapses.size() << endl;
    synapses.clear();
}

int LIFneuron::getNeuronId() {
    return neuronId;
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

void LIFneuron::setPresynapticLink(shared_ptr<LIFneuron> preNeuron) { 
    int delay;
    for (int i = 0; i < multisynapses; i++) {
        delay = randomNumber(delayMin, delayMax);
        synapses.emplace_back(make_shared<Synapse>(preNeuron, delay, dt, lambdaX, alpha));
    }
}

void LIFneuron::inhibitNeuron(int t) {
    inRefraction = true;
    timeLastSpike = t;
    spike = 0;
}

int LIFneuron::updateNeuronState(int t) {
    int s = 0;
    double forcingFunction = 0.0;
    double aux;
    maxPreX = -DBL_MAX;
    minPreX = DBL_MAX;
    maxWeight = -DBL_MAX; 
    minWeight = DBL_MAX;
    shared_ptr<LIFneuron> preNeuron = nullptr;

    int winner = -1;
    double vMaxAux = -DBL_MAX;
    
    if (!synapses.empty()) {
        if (learning) {
            for (unsigned long i = 0; i < synapses.size(); i++) {
                if (synapses[i]) {
                    preNeuron = synapses[i]->getPreNeuron();
                    if (preNeuron->getSpike() == 1) {
                        if (preNeuron->getVMax() > vMaxAux) {
                            winner = i;
                            vMaxAux = preNeuron->getVMax();
                            // cout << "Winner synapse: " << preNeuron->getNeuronId() << " " << winner << endl;
                        }
                    }
                } else {
                    std::cerr << "Error: synapse is null" << std::endl;
                }
            }
        }

        if (winner != -1) { // There is a winner synapse
            for (unsigned long i = 0; i < synapses.size(); i++) {
                if (synapses[i]) {
                    preNeuron = synapses[i]->getPreNeuron();
                    if (i == (unsigned long)winner) {
                        // cout << "Neuron " << neuronId << " preNeuron: " << preNeuron->getNeuronId() << endl;
                        synapses[winner]->updateSpikeAtributes();
                        s = synapses[winner]->obtainSpike();
                        synapses[winner]->updatePresinapticTrace(s);
                        forcingFunction += synapses[winner]->updateForcingFunction(s);

                        preNeuron->STDP();
                        // cout << "Neuron " << neuronId << " winner synapse: " << winner << endl;
                    } else {
                        preNeuron->inhibitNeuron(t);
                        synapses[i]->updateSpikeAtributes();
                        s = synapses[i]->obtainSpike();
                        synapses[i]->updatePresinapticTrace(s);
                        forcingFunction += synapses[i]->updateForcingFunction(s);
                    }

                    aux = synapses[i]->getPreSynapticTrace();
                    if (aux > maxPreX) maxPreX = aux;
                    if (aux < minPreX) minPreX = aux;
                    aux = synapses[i]->getWeight();
                    if (aux > maxWeight) maxWeight = aux;
                    if (aux < minWeight) minWeight = aux;

                } else {
                    std::cerr << "Error: synapse is null" << std::endl;
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
                } else {
                    std::cerr << "Error: synapse is null" << std::endl;
                }
            }
        }
    }

    spike = updateMembranePotential(forcingFunction, t);

    return spike;
}

int LIFneuron::updateMembranePotential(double forcingFunction, int t) { // Check this make private
    int s = 0; // s(t) = 0
    if (inRefraction) {
        if (t - timeLastSpike >= tRefr) inRefraction = false;
        else return s; // s(t) = 0
    }

    // v += (((-v) + vRest) + forcingFunction) * (dt / lambdaV);

    v += exp(-dt/lambdaV) * v + forcingFunction; // lambdaV = exp(-dt/tauM) && tauM = lambdaV.value(input parameter)

    // double decay = exp(-dt / lambdaV);
    // v = decay * (v - vRest) + vRest + forcingFunction;

    // cout << " Neuron " << neuronId << " (v: " << v << " ff: " << forcingFunction << ")" << endl;
    v = (v < vRest) ? vRest : v;

    if (v >= vTh) {
        s = 1; // s(t) = 1
        vMax = v;
        v = vReset;
        inRefraction = true;
        timeLastSpike = t;
        // cout << " Neuron " << neuronId << " fired at iteration " << t << " | V = " << v << " f: " << forcingFunction << endl;
        // STDP();
    }

     return s;
}

void LIFneuron::STDP() {
    double LTP, LTD = 0.0;
    double winit, weight, normPreX = 0.0;
    double a = 0.0; // Check this use as parameter
    double learningRate = 0.1; // Check this use as parameter
    double lTh = 0.05; // Check this use as parameter
    double mse = 0.0;

    for (unsigned long i = 0; i < synapses.size(); i++) {
        winit = synapses[i]->getWinit();
        weight = synapses[i]->getWeight();
        normPreX = synapses[i]->getNormPreSynapticTrace(minPreX, maxPreX);
        LTP = exp(-weight + winit) * exp(normPreX) - a; // LTPw *LTPx
        LTD = -exp(weight - winit) * exp(1 - normPreX) - a; // LTDw *LTDx
        synapses[i]->updateWeight(learningRate * (LTP + LTD));
        mse += MSE(i, normPreX);
        // if (normPreX < 0 || normPreX > 1) cout << neuronId << " PreSynapticTrace: " << synapses[i]->getPreSynapticTrace() << endl;
    }
    
    mse /= synapses.size();

    if (mse > lTh) {
        learning = false;
        cout << "Neuron " << neuronId << " stopped learning with MSE: " << mse << endl;
    }
}

double LIFneuron::MSE(unsigned long index, double normPreX) {
    double normWeight = 0.0;

    normWeight = synapses[index]->getNormWeight(minWeight, maxWeight);
    // if (normWeight < 0 || normWeight > 1) cout << neuronId << " NormWeight: " << normWeight << endl;

    return pow((normPreX - normWeight), 2);
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

void LIFneuron::getFirstWeight() {
    if (!synapses.empty()) {
        for (unsigned long i = 0; i < synapses.size() / 2; i++) {
            if (synapses[i]) {
                cout << "Neuron " << neuronId << " synapse " << i << " weight: " << synapses[i]->getWeight() << endl;
            }
        }
        // return synapses[0]->getWeight();
    }
    // return 0.0; 
}

int LIFneuron::gatherSpike(int t) {
    int s = 0;
    shared_ptr<LIFneuron> preNeuron = nullptr;

    for (unsigned long i = 0; i < synapses.size(); i++) {
        synapses[i]->updateSpikeAtributes();
        s = synapses[i]->obtainSpike();
        
        if (s == 1) {
            preNeuron = synapses[i]->getPreNeuron();
            preNeuron->STDP();
        }
    }

    return s;
}