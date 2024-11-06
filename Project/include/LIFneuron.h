#pragma once

#include "Globals.h"

#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

// Constructor for LIF neuron
class LIFneuron {
private: 
    // Neuron parameters
    double v; // Membrane potential
    double vTh; // Threshold potential
    double vRest; // Leak reversal potential
    double vReset; // Reset potential
    double lambdaV; // Membrane time constant
    double iSyn; // Leak conductance
    double tRefr; // Refractory period
    double lambdaX; // Trace time constant -- REMOVE
    double alpha; // Scaling factor for traces

    bool inRefraction; // Refractory state
    double timeLastSpike; // Last spike time
    double dt; // Time step -- REMOVE

    vector<LIFneuron *> postNeurons; // Post-synaptic neurons

public:
    LIFneuron(double vTh_ = V_TH, double vRest_ = V_REST, double vReset_ = V_RESET, double lambdaV_ = LAMBDA_V, double tRefr_ = T_REFR, double dt_ = DT, double lambdaX_ = LAMBDA_X, double alpha_ = ALPHA);
    double getMembranePotential();
    void setPostSynapticLink(LIFneuron &postNeuron);
    int updateMembranePotential(double inputCurrent, double time);
};