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
    double dt; // Time step
    int sumDelays; // Sum of synaptic delays
    int numSpikes; // Amount of spikes
    double weight; // Weight of the synapse
    double delay; // Tau of the synapse
    double preSynapticTrace; // Pre-synaptic trace
public:
    Synapse(LIFneuron &postNeuron_, double dt_);
    LIFneuron * getPostNeuron();
    double getWeight();
    double getDelay();
    double getPreSynapticTrace();
    int getSpike();
    void setWeight(double weight_);
    void setDelay(double delay_);
    void setPresynapticTrace(double preSynapticTrace_);
    void setSpikeAtributes(int sumDelays_, int numSpikes_);
};