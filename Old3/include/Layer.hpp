#pragma once

#include "LIFneuron.hpp"
#include "Globals.hpp"
#include "Utils.hpp"

#include <random>

using namespace std;

// struct TopologyParameters;

class Layer {
private:
    string type;
    int numNeurons;
    string connections;
    int multisynapses;
    vector<pair<int, int>> sparseConnections;

    // vector<LIFneuron> neurons;
    vector<shared_ptr<LIFneuron>> neurons;
public:
    // Layer(string type_ = UNDEFINED, int numNeurons_ = NONE, vector<pair<vector<double>, int>> neuronParams_ = {{{}, NONE}}, string connections_ = UNDEFINED, int multisynapses_ = NONE, pair<int, int> delayRange_ = {NONE, NONE}, vector<pair<int, int>> sparseConnections_ = {{NONE, NONE}});
    // Layer(const TopologyParameters &topology, const vector<pair<NeuronParameters, int>> &neuronParams, int dt);
    Layer(const TopologyParameters &topology, const vector<pair<NeuronParameters, int>> &neuronParams, int dt);
    ~Layer();
    // Layer(const Layer&) = delete;
    // Layer& operator=(const Layer&) = delete;
    
    string getType();
    int getNumNeurons();
    string getConnections();
    int getMultisynapses();
    vector<pair<int, int>>& getSparseConnections();
    shared_ptr<LIFneuron> getNeuron(int i);
    // vector<LIFneuron>& getNeurons();
    void getNeuronsType();

    void setPresynapticLinks(Layer &preLayer);
    void feedForward(int layerId, int t);
};