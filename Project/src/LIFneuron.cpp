#include "../include/LIFneuron.h"

LIFneuron::LIFneuron(double vTh_, double vRest_, double vReset_, double lambdaV_, double tRefr_, double dt_, double lambdaX_, double alpha_)
    : v(vRest_), vTh(vTh_), vRest(vRest_), vReset(vReset_), lambdaV(lambdaV_), tRefr(tRefr_), dt(dt_), lambdaX(lambdaX_), alpha(alpha_) {}

double LIFneuron::getMembranePotential() {
    return v;
}

void LIFneuron::setPostSynapticLink(LIFneuron &postNeuron) {
    postNeurons.push_back(&postNeuron);
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