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

    spike = 0;
}

LIFneuron::~LIFneuron() {
    // cout << "Destroying LIFneuron with ID: " << neuronId << endl;
    // cout << "Number of synapses: " << synapses.size() << endl;
    synapses.clear();
}

int LIFneuron::getType() {
    return type;
}

double LIFneuron::getMembranePotential() {
    return v;
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
        // delay = randomNumber(delayRange.first, delayRange.second);
        delay = randomNumber(delayMin, delayMax);
        // cout << delay << dt << endl;
        // shared_ptr<Synapse> synapse = make_shared<Synapse>(preNeuron, delay, dt, lambdaX, alpha);
        // synapses.emplace_back(synapse);
        synapses.emplace_back(make_shared<Synapse>(preNeuron, delay, dt, lambdaX, alpha));
        // synapses.emplace_back(preNeuron, delay, dt, lambdaX, alpha);
        // Synapse synapse(preNeuron, delay, dt, lambdaX, alpha);
        // synapses.push_back(synapse);
    }
}

// void LIFneuron::updateSpikeAtributes() {
//     for (int i = 0; i < synapses.size(); i++) {
//         synapses[i].updateSpikeAtributes();
//     }
// }

// void LIFneuron::updatePresinapticTrace() {
//     for (int i = 0; i < synapses.size(); i++) {
//         synapses[i].updatePresinapticTrace();
//     }
// }

// double LIFneuron::updateForcingFunction() {
//     double forcingFunction = 0.0;

//     for (int i = 0; i < synapses.size(); i++) {
//         forcingFunction += synapses[i].updateForcingFunction();
//     }
//     return forcingFunction;
// }

int LIFneuron::updateNeuronState(int t) {
    // int spike = 0;
    double forcingFunction = 0.0;
    double aux;
    maxPreX = -DBL_MAX;
    minPreX = DBL_MAX;
    maxWeight = -DBL_MAX; 
    minWeight = DBL_MAX;

    // if (synapses.empty()) {
    //     std::cerr << "Neuron " << neuronId << " has no synapses!" << std::endl;
    // }

    for (int i = 0; i < synapses.size(); i++) { // Check this for para la multisinapsis
        
        // if (!synapses[i]) {
        //     std::cerr << "Null synapse at index " << i << " in neuron " << neuronId << std::endl;
        //     continue;
        // }
        // forcingFunction += synapses[i]->update();

        if (!synapses.empty() && synapses[i]) {
            forcingFunction += synapses[i]->update();
            // forcingFunction = synapses[i]->update();
            aux = synapses[i]->getPreSynapticTrace();
            if (aux > maxPreX) maxPreX = aux;
            if (aux < minPreX) minPreX = aux;
            aux = synapses[i]->getWeight();
            if (aux > maxWeight) maxWeight = aux;
            if (aux < minWeight) minWeight = aux;
        } else {
            std::cerr << "Error: synapse is null" << std::endl;
        }

        // forcingFunction = synapses[i]->update();


        // synapses[i].updatePresinapticTrace();
        // forcingFunction += synapses[i].updateForcingFunction();
        // synapses[i].updateSpikeAtributes(); // Check this maybe it should be outside the loop
    }

    spike = updateMembranePotential(forcingFunction, t);

    return spike;
}

// Change the membrane potential formula to a discrete version
// Check the membrane potential never goes below the reset potential
int LIFneuron::updateMembranePotential(double forcingFunction, int t) { // Check this make private
    int s = 0; // s(t) = 0
    // cout << "Esto es " << t << " - " << timeLastSpike << " = " << t - timeLastSpike << endl;
    if (inRefraction) {
        if (t - timeLastSpike >= tRefr) inRefraction = false;
        else return s; // s(t) = 0
    }

    // v += (((-v) + vRest) + forcingFunction) * (dt / lambdaV);

    v += exp(-dt/lambdaV) * v + forcingFunction; // lambdaV = exp(-dt/tauM) && tauM = lambdaV.value(input parameter)

    // double decay = exp(-dt / lambdaV);
    // v = decay * (v - vRest) + vRest + forcingFunction;

    cout << " Neuron " << neuronId << " (v: " << v << " ff: " << forcingFunction << ")" << endl;
    v = (v < vRest) ? vRest : v;

    if (v >= vTh) {
        s = 1; // s(t) = 1
        v = vReset;
        inRefraction = true;
        timeLastSpike = t;
        cout << " Neuron " << neuronId << " fired at iteration " << t << " | V = " << v << " f: " << forcingFunction << endl;

        STDP();
    }

     return s;
}

void LIFneuron::STDP() {
    double LTP = 0.0;
    double LTD = 0.0;
    double weight = 0.0;
    double winit = 0.0;
    double normPreX = 0.0;
    double a = 0.0; // Check this use as parameter
    double learningRate = 0.1; // Check this use as parameter

    for (int i = 0; i < synapses.size(); i++) {
        winit = synapses[i]->getWinit();
        weight = synapses[i]->getWeight();
        normPreX = synapses[i]->getNormPreSynapticTrace(minPreX, maxPreX);

        // LTD = exp(-weight-winit);
        // LTP = exp(weight-winit);
        LTP = exp(-weight + winit) * exp(normPreX) - a; // LTPw *LTPx
        LTD = -exp(weight - winit) * exp(1 - normPreX) - a; // LTDw *LTDx
        synapses[i]->setWeight(learningRate * (LTP + LTD));
        // print weight
        // cout << "Weight: " << weight << endl;
    }
}