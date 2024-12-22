#pragma once

#include "LIFneuron.h"
#include "Globals.h"

#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

class LIFneuron;

class Synapse {
private:
    // LIFneuron * postNeuron; // Post-synaptic neuron
    LIFneuron * preNeuron; // Pre-synaptic neuron
    int dt; // Time step
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces
    
    int sumCycles; // Sum of synaptic delays
    int numSpikes; // Amount of spikes

    double weight; // Weight of the synapse
    int delay; // Tau of the synapse
    int cycles; // Time constant of the synapse
    double preSynapticTrace; // Pre-synaptic trace
public:
    Synapse(LIFneuron &preNeuron_, int delay_ = MIN_DELAY, int dt_ = DT, double lambdaX_ = LAMBDA_X, double alpha_ = ALPHA);
    double getWeight();
    int getDelay();
    double getPreSynapticTrace();
    
    int obtainSpike();
    void updateSpikeAtributes();
    void updatePresinapticTrace();
    double updateForcingFunction();
};