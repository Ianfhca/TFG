#pragma once

#include "Layer.hpp"
#include "Globals.hpp"
#include "Utils.hpp"
#include "Dataset.hpp"

#include <fstream>
#include <sstream>

#include <functional>
#include <unordered_map>
#include <deque>
#include <unordered_set>
#include <memory>

using namespace std;

class SNN {
private:
    string topologyFile; // Name of the topology file
    string baseName; // Base name for output files
    unsigned long time; // Total time for the simulation
    unsigned long dt; // Time step for the simulation
    string timeUnit; // Unit of time ("m", "s", "ms", "us", "ns") 
    string dtUnit; // Unit of time step ("m", "s", "ms", "us", "ns")
    string tRefrUnit; // Unit of refractory period ("m", "s", "ms", "us", "ns")
    string delayUnit;// Unit of delay ("m", "s", "ms", "us", "ns")
    int height; // Height of the input layer
    int width; // Width of the input layer
    int channels; // Number of channels in the input layer
    int maxDelay; // Maximum delay for synapses

    string mode; // Mode of operation ("train" or "test")

    vector<NeuronParameters> neuronParams; // Parameters for neurons

    vector<shared_ptr<Layer>> layers; // Layers of the network

    vector<deque<int>> inputSpikes; // Input spikes for each layer

    void parseParameters(const string &line);
    void parseHyperparameters(const string &line, NeuronParameters &neuron);
    void parseTopology(const string &line, TopologyParameters &topology);
    void parseInput(const string &line);
    vector<string> readTrialsList(const string& filename);
    string generateCSVFilename(const string& aedatFilename);
    int processGestureData(const SpikeCubePolarity& spikeData, int gestureClass, unsigned long& symTime, unsigned long symCap);
public:
    SNN();
    
    ~SNN();
    
    int initNetwork(char &filename, string &nMode);
    void linkLayers();
    void viewTopology();
    void viewInputSpikes();
    void trainNetwork();
    void saveWeights();
    int loadWeights(const int numFile);
    void testNetwork();
};