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
    // weak_ptr<LIFneuron> preNeuron;
    shared_ptr<LIFneuron> preNeuron;
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces
    double weight; // Weight of the synapse
    int delay; // Tau of the synapse
    int dt; // Time step

    // int sumCycles; // Sum of synaptic delays
    // int numSpikes; // Amount of spikes
    // double winit; // Initial weight
    
    int cycles; // Time constant of the synapse
    double preSynapticTrace; // Pre-synaptic trace

    // Queue spikesQ;
    vector<int> spikesQ;
public:
    // Synapse(LIFneuron &preNeuron_, int delay_ = MIN_DELAY, int dt_ = DT, double lambdaX_ = LAMBDA_X, double alpha_ = ALPHA);
    Synapse(shared_ptr<LIFneuron> preNeuron_, double lambdaX_, double alpha_, double weight_, int delay_, int dt_);
    // Synapse(shared_ptr<LIFneuron> preNeuron_, int delay_, int dt_, double lambdaX_, double alpha_);

    ~Synapse();

    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;

    // double getWinit();
    double getWeight();
    int getDelay();
    double getPreSynapticTrace();
    double getNormPreSynapticTrace(double minPreX, double maxPreX);
    double getNormWeight(double minWeight, double maxWeight);
    // double getSpike();
    // double getMembranePotential();
    shared_ptr<LIFneuron> getPreNeuron();
    void setWeight(double weight_);
    // void initializeWeights(int fanIn);
    void updateWeight(double deltaWeight);
    
    void updateSpikeAtributes();
    int obtainSpike();
    // double update();
    // bool updateSpikeAtributes();
    void updatePresinapticTrace(int spike);
    double updateForcingFunction(int spike);
    // void inhibitNeuron(int t);
    // void STDP(unsigned long index);
};