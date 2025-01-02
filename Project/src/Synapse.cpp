#include "../include/Synapse.h"

Synapse::Synapse(LIFneuron &preNeuron_, int delay_, int dt_, double lambdaX_, double alpha_) {
    // postNeuron = nullptr;
    preNeuron = &preNeuron_;
    dt = dt_;
    lambdaX = lambdaX_;
    alpha = alpha_;
    weight = 0.5;
    delay = delay_;
    cycles = (dt == delay) ? 1 : (dt / delay) + 1;
    preSynapticTrace = 0.0;
    sumCycles = 0;
    numSpikes = 0;
}

double Synapse::getWeight() {
    return weight;
}

int Synapse::getDelay() {
    return delay;
}

double Synapse::getPreSynapticTrace() {
    return preSynapticTrace;
}

int Synapse::obtainSpike() {
    int spike = 0;

    if (numSpikes > 0) {
        if (numSpikes == 1) spike = (sumCycles == numSpikes) ? 1 : 0;
        else spike = (sumCycles % numSpikes == 0) ? 1 : 0;
        
        sumCycles -= 1;
        numSpikes -= spike;
    }

    return spike;
}

void Synapse::updateSpikeAtributes() {
    // cout << "PreNeuron: " << preNeuron->getSpike() << endl;
    if (preNeuron != nullptr) {
        if (preNeuron->getSpike() == 1) {
            sumCycles += cycles;
            numSpikes += 1;
        }
    } else {
        std::cerr << "Error: preNeuron is null" << std::endl;
    }
}

void Synapse::updatePresinapticTrace() {
    // preSynapticTrace[i][j][d] = (-preSynapticTrace[i][j][d] + (alpha * neurons[j].obtainSpike(d))) * (dt / lambdaX);
    preSynapticTrace = (-preSynapticTrace + (alpha * obtainSpike())) * (dt / lambdaX);
}

double Synapse::updateForcingFunction() {
    // forcingFunction += neurons[j].obtainSpike(d) * weights[i][j][d] - preSynapticTrace[i][j][d];
    int spike = obtainSpike();
    // cout << "Spike: " << spike << " pS:" << preSynapticTrace << endl;
    return (spike * weight - preSynapticTrace);
    // return (obtainSpike() * weight - preSynapticTrace);
}