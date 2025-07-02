#pragma once

// #include <opencv2/core.hpp>
// #include <opencv4/opencv2/core.hpp>
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

class Layer {
private:
    string type; // Type of the layer ("Input", "Hidden", ..., "Output")
    int height; // Height of the layer
    int width;  // Width of the layer
    int channels; // Number of channels in the layer
    int numNeurons; // Total number of neurons in the layer
    int neuronType; // Type of neurons in the layer
    string connections; // Type of connections ("local", "global", "sparse")
    int rDim; // Size of the local receptive field (kernel size)
    int multisynapses; // Number of synapses per neuron

    vector<pair<int, int>> sparseConnections; // Sparse connections (from neuron index to pre-neuron index)

    vector<shared_ptr<LIFneuron>> neurons; // Neurons in the layer

    // vector<unsigned long> spikeHistory;
public:
    Layer(const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt);
    
    ~Layer();
    
    string getType();
    int getHeight();
    int getWidth();
    int getChannels();
    int getNumNeurons();
    int getNeuronsType();
    string getConnections();
    int getRDim();
    int getMultisynapses();
    shared_ptr<LIFneuron> getNeuron(int i);
    vector<pair<int, int>>& getSparseConnections();    

    void setPresynapticLinks(Layer &preLayer);
    void feedForward(const string& baseName, int classLabel, int t);

    void saveWeights(const string& baseName, const int layerId);
    void loadWeights(const std::string& baseName, const int layerId);

    // void showSpikeHistory();
};