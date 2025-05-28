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
    string type;
    int height;
    int width;
    int channels;
    int numNeurons;
    int neuronType;
    string connections;
    int rDim;
    // int kDim;
    int multisynapses;
    vector<pair<int, int>> sparseConnections;

    // vector<LIFneuron> neurons;
    vector<shared_ptr<LIFneuron>> neurons;
    // vector<vector<uchar>> spikeHistory;
    vector<unsigned long> spikeHistory;
public:
    // Layer(string type_ = UNDEFINED, int numNeurons_ = NONE, vector<pair<vector<double>, int>> neuronParams_ = {{{}, NONE}}, string connections_ = UNDEFINED, int multisynapses_ = NONE, pair<int, int> delayRange_ = {NONE, NONE}, vector<pair<int, int>> sparseConnections_ = {{NONE, NONE}});
    // Layer(const TopologyParameters &topology, const vector<pair<NeuronParameters, int>> &neuronParams, int dt);
    Layer(const TopologyParameters &topology, const NeuronParameters &neuronParams, int dt);
    ~Layer();
    // Layer(const Layer&) = delete;
    // Layer& operator=(const Layer&) = delete;
    
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
    // vector<LIFneuron>& getNeurons();
    

    void setPresynapticLinks(Layer &preLayer);
    void feedForward(const string& mode, int classLabel, int t);

    void saveWeights(const string& baseName, const int layerId);
    void loadWeights(const std::string& baseName, const int layerId);

    void showSpikeHistory();

    // void visualizeSpikes(int t);
};