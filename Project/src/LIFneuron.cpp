#include "../include/LIFneuron.h"

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
    tRefr = tRefr_;
    lambdaX = lambdaX_;
    alpha = alpha_;
    dt = dt_;

    inRefraction = false;
    timeLastSpike = 0;

    spike = 0;
}

LIFneuron::~LIFneuron() {
    cout << "Destroying LIFneuron with ID: " << neuronId << endl;
    cout << "Number of synapses: " << synapses.size() << endl;
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

    for (int i = 0; i < synapses.size(); i++) {
        if (!synapses.empty() && synapses[i]) {
            forcingFunction = synapses[i]->update();
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

    if (inRefraction) {
        if (t - timeLastSpike >= tRefr) inRefraction = false;
        else return s; // s(t) = 0
    }

    // v += (((-v) + vRest) + forcingFunction) * (dt / lambdaV);

    v += exp(-dt/lambdaV) * v + forcingFunction; // lambdaV = exp(-dt/tauM) && tauM = lambdaV.value(input parameter)

    // cout << " v: " << v << " ff: " << forcingFunction << endl;
    v = (v < vRest) ? vRest : v;

    if (v >= vTh) { // Check the firing threshold
        s = 1; // s(t) = 1
        v = vReset;
        inRefraction = true;
        timeLastSpike = t;
        cout << " Neuron " << neuronId << " fired at " << t << " ms" << " #V = " << v << " f: " << forcingFunction << endl;
    }

     return s;
}