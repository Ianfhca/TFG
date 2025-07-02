/**
 * @file Layer.hpp
 * @author Ian Fernandez Hermida
 * @date 2025
 * @brief Defines the Layer class, which contains and manages a collection of LIF neurons and their interconnections.
 */

#pragma once

#include "LIFneuron.hpp"
#include "Globals.hpp"
#include "Utils.hpp"

#include <random>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

using namespace std;

/**
 * @class Layer
 * @brief Represents a layer in the Spiking Neural Network.
 *
 * Each layer contains a grid of LIF neurons and manages their spatial arrangement, connections
 * to previous layers, and interactions (weight saving/loading, feedforward logic, etc.).
 */
class Layer {
private:
    /** @brief Type of the layer (Input, Hidden, ..., Output). */
    string type;
    /** @brief Height of the layer. */
    int height;
    /** @brief Width of the layer. */
    int width;
    /** @brief Number of channels in the layer. */
    int channels;
    /** @brief Total number of neurons in the layer. */
    int numNeurons;
    /** @brief Type index of the neurons in the layer. */
    int neuronType;
    /** @brief Connection scheme: local, global, or sparse. */
    string connections;
    /** @brief Size of the local receptive field for local connections. */
    int rDim;
    /** @brief Number of synapses per-neuron. */
    int multisynapses;

    /** @brief Sparse connectivity pairs (preNeuronIdx → neuronIdx). */
    vector<pair<int, int>> sparseConnections;

    /** @brief Collection of neurons in this layer. */
    vector<shared_ptr<LIFneuron>> neurons;

    // vector<unsigned long> spikeHistory;
public:
    /**
     * @brief Constructs a new Layer object.
     * @param topology Topology parameters describing structure.
     * @param neuronParams Parameters defining the neuron model.
     * @param dt Simulation time step.
     */
    Layer(const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt);

    /** @brief Destructor. Cleans up internal neuron structures. */
    ~Layer();

    /** @return The type of the layer. */
    string getType();
    /** @return Height of the layer. */
    int getHeight();
    /** @return Width of the layer. */
    int getWidth();
    /** @return Number of channels in the layer. */
    int getChannels();
    /** @return Total number of neurons. */
    int getNumNeurons();
    /** @return Neuron type index. */
    int getNeuronsType();
    /** @return Type of connections. */
    string getConnections();
    /** @return Receptive field size for local connections. */
    int getRDim();
    /** @return Number of synapses per neuron. */
    int getMultisynapses();
    /**
     * @brief Returns a shared pointer to a specific neuron.
     * @param i Index of the neuron.
     * @return Shared pointer to the i-th neuron.
     */
    shared_ptr<LIFneuron> getNeuron(int i);
    /** @return Reference to the list of sparse connections. */
    vector<pair<int, int>>& getSparseConnections();  

    /**
     * @brief Establishes synaptic links from a previous layer.
     * @param preLayer Reference to the presynaptic layer.
     */
    void setPresynapticLinks(Layer &preLayer);
    /**
     * @brief Updates all neurons in the layer for a given time step.
     * @param baseName Base path to output files.
     * @param classLabel Class label for saving spikes information training/testing.
     * @param t Current simulation time step.
     */
    void feedForward(const string& baseName, int classLabel, int t);

    /**
     * @brief Saves synaptic weights of all neurons to file.
     * @param baseName Base path for output.
     * @param layerId Index of the layer.
     */
    void saveWeights(const string& baseName, const int layerId);
    /**
     * @brief Loads synaptic weights from file.
     * @param baseName Base path for input.
     * @param layerId Index of the layer.
     * @return 0 on success, 1 on failure.
     */
    int loadWeights(const std::string& baseName, const int layerId);

    // void showSpikeHistory();
};