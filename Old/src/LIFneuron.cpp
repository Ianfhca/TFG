#include "../include/LIFneuron.h"

// LIFneuron::LIFneuron(double vTh_, double vRest_, double vReset_, double lambdaV_, double tRefr_, double dt_, double lambdaX_, double alpha_)
//     : v(vRest_), vTh(vTh_), vRest(vRest_), vReset(vReset_), lambdaV(lambdaV_), tRefr(tRefr_), dt(dt_), lambdaX(lambdaX_), alpha(alpha_) {}

LIFneuron::LIFneuron(int multisynaptic_, double vTh_, double vRest_, double vReset_, double lambdaV_, double tRefr_, double dt_, double lambdaX_, double alpha_) {
    v = vRest_;
    vTh = vTh_;
    vRest = vRest_;
    vReset = vReset_;
    lambdaV = lambdaV_;
    tRefr = tRefr_;
    dt = dt_;
    lambdaX = lambdaX_;
    alpha = alpha_;
    multisynaptic = multisynaptic_;

    inRefraction = false;
    timeLastSpike = 0.0;

    postsynapticSpike = 0;
    spikes.resize(multisynaptic, make_pair(0, 0));
    // sumTau = 0;
    // numSpikes = 0;
}

double LIFneuron::getMembranePotential() {
    return v;
}

int LIFneuron::getPostsynapticSpike() {
    return postsynapticSpike;
} 

int LIFneuron::getSpike(int ms) {
    int spike = 0;
    if (spikes[ms].second > 0) {
        spike = spikes[ms].first % spikes[ms].second == 0 ? 1 : 0;
        spikes[ms].first -= 1; // dt Check this
        spikes[ms].second -= spike;
    }

    return spike;
    // return sumTau % numSpikes == 0 ? 1 : 0;
}

void LIFneuron::setPostSynapticLink(LIFneuron &postNeuron) {
    postNeurons.push_back(&postNeuron);
}

void LIFneuron::setPostsynapticSpike(int spike) {
    postsynapticSpike = spike;
}

void LIFneuron::setSpikeAtributes(int sumTau, int numSpikes, int multisynaptic) {
    spikes[multisynaptic].first += sumTau - spikes[multisynaptic].first;
    spikes[multisynaptic].second = numSpikes;
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