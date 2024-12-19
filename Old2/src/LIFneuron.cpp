#include "../include/LIFneuron.h"

// LIFneuron::LIFneuron(double vTh_, double vRest_, double vReset_, double lambdaV_, double tRefr_, double dt_, double lambdaX_, double alpha_)
//     : v(vRest_), vTh(vTh_), vRest(vRest_), vReset(vReset_), lambdaV(lambdaV_), tRefr(tRefr_), dt(dt_), lambdaX(lambdaX_), alpha(alpha_) {}

LIFneuron::LIFneuron(int multisynapses_, double vTh_, double vRest_, double vReset_, double lambdaV_, double tRefr_, double dt_, double lambdaX_, double alpha_) {
    v = vRest_;
    vTh = vTh_;
    vRest = vRest_;
    vReset = vReset_;
    lambdaV = lambdaV_;
    tRefr = tRefr_;
    dt = dt_;
    lambdaX = lambdaX_;
    alpha = alpha_;
    multisynapses = multisynapses_;

    inRefraction = false;
    timeLastSpike = 0.0;

    postsynapticSpike = 0;
    spikes.resize(multisynapses, make_pair(0, 0));
}

double LIFneuron::getMembranePotential() {
    return v;
}

vector<Synapse> LIFneuron::getSynapses() {
    return synapses;
}

int LIFneuron::getPostsynapticSpike() {
    return postsynapticSpike;
} 

void LIFneuron::setPostsynapticSpike(int spike) {
    postsynapticSpike = spike;
}

int LIFneuron::getSpike(int multisynapticLink) {
    return synapses[multisynapticLink].getSpike();
}

void LIFneuron::setPostsynapticLink(LIFneuron &postNeuron) {
    // LIFneuron *postNeuronPtr = &postNeuron;
    for (int i = 0; i < multisynapses; i++) {
        Synapse synapse(postNeuron, dt);
        synapses.push_back(synapse);
    }
}

void LIFneuron::setSpikeAtributes(int sumDelays, int numSpikes, int multisynapticLink) {
    synapses[multisynapticLink].setSpikeAtributes(sumDelays, numSpikes);
}

void LIFneuron::setPresynapticTrace(int multisynapticLink) {
    double presynapticTrace = (synapses[multisynapticLink].getPreSynapticTrace());
    int spike = synapses[multisynapticLink].getSpike();
    synapses[multisynapticLink].setPresynapticTrace(((-presynapticTrace) + (alpha * spike)) * (dt / lambdaX));
}

double LIFneuron::updateForcingFunction(int multisynapticLink) {
    return synapses[multisynapticLink].getSpike() * synapses[multisynapticLink].getWeight() - synapses[multisynapticLink].getPreSynapticTrace();
}

int LIFneuron::updateMembranePotential(double forcingFunction, double t) {
    if (inRefraction) { // Handle refraction
        if (t - timeLastSpike >= tRefr) {
            inRefraction = false;
        } else {
            return 0; // s(t) = 0
        }
    }

    v += (-(v - vRest) + forcingFunction) * (dt / lambdaV);

    if (v >= vTh) { // Check the firing threshold
        v = vReset;
        inRefraction = true;
        timeLastSpike = t;
        std::cout << "Neuron fired at time " << t << " ms" << std::endl;
        return 1; // s(t) = 1
    }

    return 0; // s(t) = 0
}

// int LIFneuron::getSpike(int ms) {
//     int spike = 0;
//     if (spikes[ms].second > 0) {
//         spike = spikes[ms].first % spikes[ms].second == 0 ? 1 : 0;
//         spikes[ms].first -= 1; // dt Check this
//         spikes[ms].second -= spike;
//     }

//     return spike;
//     // return sumTau % numSpikes == 0 ? 1 : 0;
// }

// void LIFneuron::setPostSynapticLink(LIFneuron &postNeuron) {
//     postNeurons.push_back(&postNeuron);
// }

// void LIFneuron::setSpikeAtributes(int sumTau, int numSpikes, int multisynaptic) {
//     spikes[multisynaptic].first += sumTau - spikes[multisynaptic].first;
//     spikes[multisynaptic].second = numSpikes;
// }