#pragma once

#include "Synapse.h"
#include "Globals.h"
#include "Utils.h"

#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

class Synapse;

// Constructor for LIF neuron
class LIFneuron {
private: 
    // Neuron parameters
    int multisynapses; // Number of synapses
    pair<int, int> delayRange; // Range of synaptic delays
    double v; // Membrane potential
    double vTh; // Threshold potential
    double vRest; // Leak reversal potential
    double vReset; // Reset potential
    double lambdaV; // Membrane time constant
    double iSyn; // Leak conductance
    int tRefr; // Refractory period
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces

    bool inRefraction; // Refractory state
    int timeLastSpike; // Last spike time
    int dt; // Time step

    int spike; // Spike value
    vector<Synapse> synapses; // Post-synaptic neurons

public:
    LIFneuron(int multisynapses_ = 1, pair<int, int> delayRange_ = {NONE, NONE}, double vTh_ = V_TH, double vRest_ = V_REST, double vReset_ = V_RESET, double lambdaV_ = LAMBDA_V, double tRefr_ = T_REFR, int dt_ = DT, double lambdaX_ = LAMBDA_X, double alpha_ = ALPHA);
    double getMembranePotential();
    int getSpike();
    void setSpike(int spike_);

    void setPresynapticLink(LIFneuron &preNeuron);
    // Join this three functions in one
    void updateSpikeAtributes();
    void updatePresinapticTrace();
    double updateForcingFunction();
    int updateMembranePotential(double forcingFunction, int time);
};