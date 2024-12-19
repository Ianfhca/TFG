#include "../include/Synapse.h"

Synapse::Synapse(LIFneuron &preNeuron_, double dt_, double lambdaX_, double alpha_) {
    postNeuron = nullptr;
    preNeuron = &preNeuron_;
    dt = dt_;
    lambdaX = lambdaX_;
    alpha = alpha_;
    weight = 0.5;
    delay = 0.1; // Distribution of delays
    preSynapticTrace = 0.0;
    sumDelays = 0;
    numSpikes = 0;

    // cout << postNeuron->getMembranePotential() << endl;
}

LIFneuron * Synapse::getPostNeuron() {
    return postNeuron;
}

double Synapse::getWeight() {
    return weight;
}

double Synapse::getDelay() {
    return delay;
}

double Synapse::getPreSynapticTrace() {
    return preSynapticTrace;
}

// void Synapse::setPostsynapticLink(LIFneuron &postNeuron_) {
//     postNeuron = &postNeuron_;
// }

void Synapse::setPresynapticLink(LIFneuron &preNeuron_) {
    preNeuron = &preNeuron_;
}

void Synapse::setWeight(double weight_) {
    weight = weight_;
}

void Synapse::setDelay(double delay_) {
    delay = delay_;
}

int Synapse::obtainSpike() {
    int spike = 0;
    if (numSpikes > 0) {
        spike = sumDelays % numSpikes == 0 ? 1 : 0;
        sumDelays -= 1; // dt Check this
        numSpikes -= spike;
    }

    return spike;
}



// void Synapse::setPresynapticTrace(double preSynapticTrace_) {
//     preSynapticTrace = preSynapticTrace_;
// }

void Synapse::setSpikeAtributes(int sumDelays_, int numSpikes_) { // CHECK THIS (new name: updateSpike)
    // if (preNeuron->getPostsynapticSpike() == 1) {
    //     sumDelays += sumDelays_;
    //     numSpikes += numSpikes_;
    // }
    sumDelays += sumDelays_ - numSpikes;
    numSpikes = numSpikes_;
}

void Synapse::updatePresinapticTrace() {
    preSynapticTrace = (-preSynapticTrace + (alpha * obtainSpike())) * (dt / lambdaX);
}