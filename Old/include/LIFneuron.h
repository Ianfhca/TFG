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
    int multisynaptic; // Number of synapses
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

    // int sumTau; // Sum of synaptic delays 
    // int numSpikes; // Number of spikes
    int postsynapticSpike; // Postsynaptic spike value
    vector<pair<int, int>> spikes; // Spike times <sum of synaptic delays, number of spikes>

    vector<LIFneuron *> postNeurons; // Post-synaptic neurons

public:
    LIFneuron(int multisynaptic = 1, double vTh_ = V_TH, double vRest_ = V_REST, double vReset_ = V_RESET, double lambdaV_ = LAMBDA_V, double tRefr_ = T_REFR, double dt_ = DT, double lambdaX_ = LAMBDA_X, double alpha_ = ALPHA);
    double getMembranePotential();
    int getPostsynapticSpike();
    int getSpike(int ms);
    void setPostSynapticLink(LIFneuron &postNeuron);
    void setPostsynapticSpike(int spike);
    void setSpikeAtributes(int st, int ns, int multisynaptic);

    int updateMembranePotential(double inputCurrent, double time);
};