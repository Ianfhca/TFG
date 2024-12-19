#include "../include/Synapse.h"

Synapse::Synapse(LIFneuron &postNeuron_, double dt_) {
    postNeuron = &postNeuron_;
    dt = dt_;
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

int Synapse::getSpike() {
    int spike = 0;
    if (numSpikes > 0) {
        spike = sumDelays % numSpikes == 0 ? 1 : 0;
        sumDelays -= 1; // dt Check this
        numSpikes -= spike;
    }

    return spike;
}

void Synapse::setWeight(double weight_) {
    weight = weight_;
}

void Synapse::setDelay(double delay_) {
    delay = delay_;
}

void Synapse::setPresynapticTrace(double preSynapticTrace_) {
    preSynapticTrace = preSynapticTrace_;
}

void Synapse::setSpikeAtributes(int sumDelays_, int numSpikes_) {
    sumDelays += sumDelays_ - numSpikes;
    numSpikes = numSpikes_;
}