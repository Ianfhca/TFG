#pragma once

#include "LIFneuron.h"
#include "Globals.h"
#include "Utils.h"

#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;

class LIFneuron; 

class Synapse {
private:
    // LIFneuron * postNeuron; // Post-synaptic neuron
    // LIFneuron * preNeuron; // Pre-synaptic neuron
    weak_ptr<LIFneuron> preNeuron;
    int dt; // Time step
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces

    int sumCycles; // Sum of synaptic delays
    int numSpikes; // Amount of spikes

    double weight; // Weight of the synapse
    int delay; // Tau of the synapse
    int cycles; // Time constant of the synapse
    double preSynapticTrace; // Pre-synaptic trace

    // Queue spikesQ;
    vector<int> spikesQ;
public:
    // Synapse(LIFneuron &preNeuron_, int delay_ = MIN_DELAY, int dt_ = DT, double lambdaX_ = LAMBDA_X, double alpha_ = ALPHA);
    Synapse(shared_ptr<LIFneuron> preNeuron_, int delay_, int dt_, double lambdaX_, double alpha_);

    ~Synapse();

    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;

    double getWeight();
    int getDelay();
    double getPreSynapticTrace();
    
    int obtainSpike();
    double update();
    void updateSpikeAtributes();
    // bool updateSpikeAtributes();
    void updatePresinapticTrace(int spike);
    double updateForcingFunction(int spike);
};