/**
 * @file Synapse.hpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Defines the Synapse class, representing the connection between two LIF neurons with delayed spike transmission.
 */

#pragma once

#include "LIFneuron.hpp"
#include "Globals.hpp"
#include "Utils.hpp"

#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;

class LIFneuron; 

/**
 * @class Synapse
 * @brief Representa una sinapsis entre una neurona pre-sináptica y una post-sináptica.
 * 
 * Se encarga de modelar el retardo sináptico, el peso, la traza pre-sináptica y las
 * funciones de plasticidad necesarias para el aprendizaje.
 */
class Synapse {
private:
    /** @brief Pre-synaptic neuron connected to the current synapse. */
    shared_ptr<LIFneuron> preNeuron;
    /** @brief Time constant for the pre-synaptic trace. */
    double lambdaX;
    /** @brief Scaling factor for the pre-synaptic trace. */
    double alpha;
    /** @brief Synaptic weight, which can be modified by learning rules. */
    double weight;
    /** @brief Delay of the synapse, in time steps. */
    int delay;
    /** @brief Time step of the simulation. */
    int dt;
    
    /** @brief Number of cycles corresponding to the delay. */
    int cycles;
    /** @brief Pre-synaptic trace, which accumulates the influence of pre-synaptic spikes. */
    double preSynapticTrace;

    /** @brief Queue of spikes from the pre-synaptic neuron, used to manage delays. */
    vector<int> spikesQ;
public:
    /**
     * @brief Constructor for the Synapse class.
     * @param preNeuron_ Pointer to the pre-synaptic neuron.
     * @param lambdaX_ Time constant for the pre-synaptic trace.
     * @param alpha_ Scaling factor for the pre-synaptic trace.
     * @param weight_ Initial synaptic weight.
     * @param delay_ Delay of the synapse in time steps.
     * @param dt_ Time step of the simulation.
     */
    Synapse(shared_ptr<LIFneuron> preNeuron_, double lambdaX_, double alpha_, double weight_, int delay_, int dt_);

    /** @brief Destructor for the Synapse class. */
    ~Synapse();

    // Disable copy constructor and assignment operator
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;

    /** @brief Get the synaptic weight. */
    double getWeight();
    /** @return The delay of the synapse. */
    int getDelay();
    /** @return The current pre-synaptic trace value. */
    double getPreSynapticTrace();
    /**
     * @brief Normalizes the pre-synaptic trace value to a range [0, 1].
     * @param minPreX Minimum value of the pre-synaptic trace in the current step.
     * @param maxPreX Maximum value of the pre-synaptic trace in the current step.
     * @return Normalized pre-synaptic trace.
     */
    double getNormPreSynapticTrace(double minPreX, double maxPreX);
    /**
     * @brief Normalizes the synaptic weight to a range [0, 1].
     * @param minWeight Minimum weight observed in the current step.
     * @param maxWeight Maximum weight observed in the current step.
     * @return Normalized synaptic weight.
     */
    double getNormWeight(double minWeight, double maxWeight);
    /** @return Pointer to the pre-synaptic neuron. */  
    shared_ptr<LIFneuron> getPreNeuron();
    /** @brief Sets the synaptic weight. */
    void setWeight(double weight_);

    /**
     * @brief Obtains the previous spike from the pre-synaptic neuron and updates it based on the current cycle.
     * @return 1 if a spike was detected, 0 otherwise.
     */
    int obtainPreviousSpike();
    /**
     * @brief Updates the pre-synaptic trace based on the current spike.
     * @param spike Current spike value (1 if spike occurred, 0 otherwise).
     * 
     * The pre-synaptic trace is updated using an exponential decay formula:
     * preSynapticTrace = decay * (-preSynapticTrace) + (alpha * spike);
     * where decay = 1 / lambdaX.
     */
    void updatePresinapticTrace(int spike);
    /**
     * @brief Updates the forcing function based on the current spike.
     * @param spike Current spike value (1 if spike occurred, 0 otherwise).
     * @return The updated forcing function value.
     * 
     * The forcing function is calculated as:
     * forcingFunction = spike * weight - preSynapticTrace
     */
    double updateForcingFunction(int spike);
    /**
     * @brief Updates the synaptic weight by applying a delta value.
     * @param deltaWeight The change in weight to be applied.
     */
    void updateWeight(double deltaWeight);
};