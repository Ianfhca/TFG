#pragma once

#include "Synapse.hpp"
#include "Globals.hpp"
#include "Utils.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <memory>
#include <cfloat>

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
    // double lambdaV; // Membrane time constant
    double tauM; // Membrane time constant
    int tRefr; // Refractory period
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces
    double winit; // Initial weight
    double learningRate; // Learning rate
    double a; // Scaling factor for weight
    double convergenceTh; // Convergence threshold
    int dt; // Time step

    double maxPreX; // Maximum pre-synaptic trace
    double minPreX; // Minimum pre-synaptic trace
    double maxWeight; // Maximum weight
    double minWeight; // Minimum weight

    bool inRefraction; // Refractory state
    int timeLastSpike; // Last spike time 
    double vMax; // Maximum membrane potential

    int spike; // Spike value 1 / 0
    bool learning; // Learning state
    bool WTArule; // Winner-take-all rule
    // vector<Synapse> synapses; // Presynaptic neurons
    vector<shared_ptr<Synapse>> synapses; // Presynaptic neurons

    int updateMembranePotential(double forcingFunction, int time);

public:
    // LIFneuron(int multisynapses_ = 1, pair<int, int> delayRange_ = {NONE, NONE}, double vTh_ = V_TH, double vRest_ = V_REST, double vReset_ = V_RESET, double lambdaV_ = LAMBDA_V, double tRefr_ = T_REFR, int dt_ = DT, double lambdaX_ = LAMBDA_X, double alpha_ = ALPHA);
    LIFneuron(int neuronId, const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt);
    // LIFneuron(int neuronId_, int type_, int multisynapses_, int delayMin_, int delayMax_, double vTh_, double vRest_, double vReset_, double lambdaV_, int tRefr_, double lambdaX_, double alpha_, int dt_);
    ~LIFneuron();
    
    int getNeuronId();
    int getType();
    double getMembranePotential();
    double getVMax();
    int getSpike();
    void setSpike(int spike_);

    void setPresynapticLink(shared_ptr<LIFneuron> preNeuron);
    // Join this three functions in one
    // void updateSpikeAtributes();
    // void updatePresinapticTrace();
    // double updateForcingFunction();
    void inhibitNeuron(int time);
    int updateNeuronState(int time);
    int gatherSpike(int time);
    // void STDP(unsigned long index);
    
    void STDP();
    // double MSE();
    double MSE(unsigned long index, double normPreX);

    // int updateMembranePotential(double forcingFunction, int time);
    void getFirstWeight();
};