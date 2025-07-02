/**
 * @file LIFneuron.hpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Defines the LIFneuron class, which simulates a spiking neuron using the Leaky Integrate-and-Fire model.
 */

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

/**
 * @class LIFneuron
 * @brief Implements a Leaky Integrate-and-Fire neuron with STDP and WTA learning mechanisms.
 *
 * This class represents an individual neuron in a Spiking Neural Network using the LIF model.
 * It supports synaptic plasticity via STDP, winner-take-all inhibition, and saving/loading synaptic weights.
 */
class LIFneuron {
private:
    /** @brief 3D location and flat index of the neuron: [i, j, channel, index]. */
    vector<int> neuronId;
    /** @brief Name of the layer the neuron belongs to. */
    string layer;
    /** @brief Type index of the neuron. */
    int type;
    /** @brief Number of synapses per presynaptic neuron. */
    int multisynapses;
    /** @brief Range of delays for synapses. */
    pair<int, int> delayRange;
    /** @brief Minimum delay for synapses. */
    int delayMin;
    /** @brief Maximum delay for synapses. */
    int delayMax;
    /** @brief Whether the STDP learning rule is enabled. */
    bool stdpRule;
    /** @brief Whether WTA competition is enabled. */
    int wtaRule;
    /** @brief Operating mode: "train" or "test". */
    string mode;

    /** @brief Membrane potential. */
    double v;
    /** @brief Spike threshold. */
    double vTh;
    /** @brief Resting potential. */
    double vRest;
    /** @brief Reset potential after spike. */
    double vReset;
    /** @brief Membrane time constant. */
    double tauM;
    /** @brief Refractory period in time steps. */
    int tRefr;
    /** @brief Presynaptic trace time constant. */
    double lambdaX;
    /** @brief Scaling factor for presynaptic trace. */
    double alpha;
    /** @brief Initial synaptic weight. */
    double winit;
    /** @brief Learning rate for STDP. */
    double learningRate;
    /** @brief Scaling constant for synaptic weight updates. */
    double aValue;
    /** @brief Threshold for mean squared error to stop learning. */
    double convergenceTh;
    /** @brief Simulation time step. */
    int dt;

    /** @brief Maximum presynaptic trace seen in current step. */
    double maxPreX;
    /** @brief Minimum presynaptic trace seen in current step. */
    double minPreX;
    /** @brief Maximum weight observed in current step. */
    double maxWeight;
    /** @brief Minimum weight observed in current step. */
    double minWeight;

    /** @brief True if neuron is in refractory state. */
    bool inRefraction;
    /** @brief Time of last spike. */
    int timeLastSpike;
    /** @brief Maximum membrane potential reached. */
    double vMax;

    /** @brief Binary spike signal (1 or 0). */
    int spike;
    /** @brief Whether current neuron is still learning. */
    bool learning;
    /** @brief Number of neurons that are postsynaptic to the current one. */
    int postNeuronAmount;
    /** @brief Remaining postsynaptic links to process. */
    int remainingLinks;
    /** @brief Whether neuron has been WTA-inhibited in the current step. */
    bool wtaInhibition;
    
    /** @brief Synapses connecting this neuron to presynaptic neurons. */
    vector<shared_ptr<Synapse>> synapses;

    /** @brief Updates the membrane potential based on input. 
     * @param forcingFunction Input from presynaptic neurons.
     * @param time Current simulation time step.
     * @return Spike signal (1 if spike, 0 otherwise).
    */
    int updateMembranePotential(double forcingFunction, int time);

public:
    /**
     * @brief Constructor.
     * @param neuronId 3D coordinates and flat index of the neuron.
     * @param topology Network topology settings.
     * @param neuronParams Neuron hyperparameters (potentials, time constants, etc.).
     * @param dt Simulation time step.
     */
    LIFneuron(vector<int> neuronId, const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt);

    /** @brief Destructor. Clears synapses. */
    ~LIFneuron();
    
    /** @return Neuron index (prints position and returns flat index). */
    int getNeuronId();
    /** @return Type of the neuron. */
    int getType();
    /** @return Current membrane potential. */
    double getMembranePotential();
    /** @return Maximum membrane potential observed. */
    double getVMax();
    /** @return Spike value. */
    int getSpike();
    /** @brief Sets the spike flag manually. */
    void setSpike(int spike_);
    /**
     * @brief Connects this neuron to a presynaptic neuron with multiple synapses.
     * @param preNeuron Pointer to the presynaptic neuron.
     * @param numNeurons Total neurons in the presynaptic layer.
     */
    void setPresynapticLinks(shared_ptr<LIFneuron> preNeuron, int numNeurons);
    /** @brief Increments count of postsynaptic links. */
    void addLink();
    /** @return Number of postsynaptic connections. */
    int getPostNeuronAmount();
    /** @return Remaining postsynaptic links to be processed. */
    int getRemainingLinks();
    /** @brief Sets the remaining links manually. */
    void setRemainingLinks(int remainingLinks_);
    /** @brief Decrements remaining links counter. */
    int decrementRemainingLinks();
    /** @return Whether the neuron has been inhibited due to WTA. */
    bool getWtaInhibition();
    /** @brief Sets WTA inhibition state. */
    void setWtaInhibition(bool wtaInhibition_);


    /** @brief Inhibits the neuron (resets membrane potential and enters refractory period). */
    void inhibitNeuron(int time);
    /**
     * @brief Updates the neuron's internal state and checks for spiking.
     * @param time Current simulation time.
     * @return 1 if the neuron spikes, 0 otherwise.
     */
    int updateNeuronState(int time);
    /** @brief Winner-Take-All logic to select the dominant input synapse. 
     * @return Index of the winning synapse, or -1 if no winner found.
    */
    long long WTA();
    /** @brief Applies STDP to all incoming synapses. */
    void STDP();
    /** @brief Evaluates learning convergence using MSE. 
     * @param mse Mean squared error of presynaptic vs. postsynaptic traces.
    */
    void MSE(double mse);

    /**
     * @brief Saves all synaptic weights to a file.
     * @param fileName Path to output file.
     * @param id Neuron ID for labeling entries.
     */
    void saveWeights(const string& fileName, int id);
    /**
     * @brief Loads synaptic weights from a file.
     * @param fileName Path to input file.
     */
    void loadWeights(const string& fileName);
};