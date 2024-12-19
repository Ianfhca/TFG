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
    LIFneuron * postNeuron; // Post-synaptic neuron
    LIFneuron * preNeuron; // Pre-synaptic neuron
    double dt; // Time step
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces
    int sumDelays; // Sum of synaptic delays
    int numSpikes; // Amount of spikes
    double weight; // Weight of the synapse
    double delay; // Tau of the synapse
    double preSynapticTrace; // Pre-synaptic trace
public:
    Synapse(LIFneuron &preNeuron_, double dt_, double lambdaX_, double alpha_);
    LIFneuron * getPostNeuron();
    double getWeight();
    double getDelay();
    double getPreSynapticTrace();
    int obtainSpike();
    // void setPostsynapticLink(LIFneuron &postNeuron_);
    void setPresynapticLink(LIFneuron &preNeuron_);
    void setWeight(double weight_);
    void setDelay(double delay_);
    void setPresynapticTrace(double preSynapticTrace_);
    void setSpikeAtributes(int sumDelays_, int numSpikes_);
    void updatePresinapticTrace();
};