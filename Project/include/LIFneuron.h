#pragma once

#include "Synapse.h"
#include "Globals.h"
#include "Utils.h"

#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

class Synapse;

class LIFneuron {

private:
    int neuronId; // Neuron ID 
    int type; // Neuron type
    int multisynapses; // Number of synapses
    pair<int, int> delayRange; // Range of synaptic delays
    int delayMin; // Minimum delay
    int delayMax; // Maximum delay
    double v; // Membrane potential
    double vTh; // Threshold potential
    double vRest; // Leak reversal potential
    double vReset; // Reset potential
    double lambdaV; // Membrane time constant
    int tRefr; // Refractory period
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces
    int dt; // Time step

    bool inRefraction; // Refractory state
    int timeLastSpike; // Last spike time
    

    int spike; // Spike value 1 / 0
    vector<Synapse> synapses; // Presynaptic neurons

    int updateMembranePotential(double forcingFunction, int time);

public:
    // LIFneuron(int multisynapses_ = 1, pair<int, int> delayRange_ = {NONE, NONE}, double vTh_ = V_TH, double vRest_ = V_REST, double vReset_ = V_RESET, double lambdaV_ = LAMBDA_V, double tRefr_ = T_REFR, int dt_ = DT, double lambdaX_ = LAMBDA_X, double alpha_ = ALPHA);
    LIFneuron(int neuronId_, int type_, int multisynapses_, int delayMin_, int delayMax_, double vTh_, double vRest_, double vReset_, double lambdaV_, int tRefr_, double lambdaX_, double alpha_, int dt_);
    int getType();
    double getMembranePotential();
    int getSpike();
    void setSpike(int spike_);

    void setPresynapticLink(LIFneuron &preNeuron);
    // Join this three functions in one
    // void updateSpikeAtributes();
    // void updatePresinapticTrace();
    // double updateForcingFunction();
    int updateNeuronState(int time);
    // int updateMembranePotential(double forcingFunction, int time);
};