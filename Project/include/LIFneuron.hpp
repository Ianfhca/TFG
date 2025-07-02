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
#include <fstream>

using namespace std;

class Synapse;

class LIFneuron {

private:
    vector<int> neuronId; // Neuron ID 
    string layer; // Layer name
    int type; // Neuron type
    int multisynapses; // Number of synapses
    pair<int, int> delayRange; // Range of synaptic delays
    int delayMin; // Minimum delay
    int delayMax; // Maximum delay
    bool stdpRule; // STDP rule
    int wtaRule; // Winner-take-all rule
    string mode; // Mode of operation (train/test)

    double v; // Membrane potential
    double vTh; // Threshold potential
    double vRest; // Leak reversal potential
    double vReset; // Reset potential
    double tauM; // Membrane time constant
    int tRefr; // Refractory period
    double lambdaX; // Trace time constant
    double alpha; // Scaling factor for traces
    double winit; // Initial weight
    double learningRate; // Learning rate
    double aValue; // Scaling factor for weight
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
    int postNeuronAmount; // Number of links from post-synaptic neurons
    int remainingLinks; // Remaining links to be used
    bool wtaInhibition; // Flag to check if the neuron has been a winner
    
    vector<shared_ptr<Synapse>> synapses; // Presynaptic neurons

    int updateMembranePotential(double forcingFunction, int time);

public:
   LIFneuron(vector<int> neuronId, const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt);
    
   ~LIFneuron();
    
    int getNeuronId();
    int getType();
    double getMembranePotential();
    double getVMax();
    int getSpike();
    void setSpike(int spike_);
    void setPresynapticLinks(shared_ptr<LIFneuron> preNeuron, int numNeurons);
    void addLink();
    int getPostNeuronAmount();
    int getRemainingLinks();
    void setRemainingLinks(int remainingLinks_);
    int decrementRemainingLinks();
    bool getWtaInhibition();
    void setWtaInhibition(bool wtaInhibition_);

    void inhibitNeuron(int time);
    int updateNeuronState(int time);
    long long WTA();
    void STDP();
    void MSE(double mse);

    void saveWeights(const string& fileName, int id);
    void loadWeights(const string& fileName);
};