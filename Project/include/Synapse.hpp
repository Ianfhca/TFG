#pragma once

#include "LIFneuron.hpp"
#include "Globals.hpp"
#include "Utils.hpp"

#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;

class LIFneuron; 

class Synapse {
private:
    shared_ptr<LIFneuron> preNeuron;
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces
    double weight; // Weight of the synapse
    int delay; // Tau of the synapse
    int dt; // Time step
    
    int cycles; // Time constant of the synapse
    double preSynapticTrace; // Pre-synaptic trace

    vector<int> spikesQ; // Queue of spikes from the pre-synaptic neuron
public:
    Synapse(shared_ptr<LIFneuron> preNeuron_, double lambdaX_, double alpha_, double weight_, int delay_, int dt_);

    ~Synapse();

    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;

    double getWeight();
    int getDelay();
    double getPreSynapticTrace();
    double getNormPreSynapticTrace(double minPreX, double maxPreX);
    double getNormWeight(double minWeight, double maxWeight);
    shared_ptr<LIFneuron> getPreNeuron();
    void setWeight(double weight_);

    int obtainPreviousSpike();

    void updatePresinapticTrace(int spike);
    double updateForcingFunction(int spike);
    void updateWeight(double deltaWeight);
};