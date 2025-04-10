#pragma once

#include "LIFneuron.h"
#include "Globals.h"

#include <iostream>
#include <cstdlib>
#include <cstdint>
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
    // uint8_t bitmap; // Bitmap for the synapse
    // uint16_t bitmap;

    double weight; // Weight of the synapse
    int delay; // Tau of the synapse
    int cycles; // Time constant of the synapse
    double preSynapticTrace; // Pre-synaptic trace
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
    bool updateSpikeAtributes();
    void updatePresinapticTrace(int spike);
    double updateForcingFunction(int spike);
};