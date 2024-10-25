#pragma once

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

    bool inRefraction; // Refractory state
    double timeLastSpike; // Last spike time
    double dt; // Time step

    vector<double> X; // Presynaptic traces
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces

    // vector<double> sHist; // Spike history

    vector<LIFneuron *> postNeurons; // Post-synaptic neurons

public:
    LIFneuron();
    LIFneuron(double vTh_, double vRest_, double vReset_, double lambdaV_, double tRefr_, double dt_, double lambdaX_, double alpha_);
    double getMembranePotential();
    void setPostSynapticLink(LIFneuron &postNeuron);
    int updateMembranePotential(double inputCurrent, double time);
    void fire(double time);
    void updateTrace(double sPre);
};