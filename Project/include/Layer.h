#pragma once

#include "LIFneuron.h"
#include "Globals.h"
#include "Utils.h"

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

    vector<LIFneuron> neurons;
public:
    // Layer(string type_ = UNDEFINED, int numNeurons_ = NONE, vector<pair<vector<double>, int>> neuronParams_ = {{{}, NONE}}, string connections_ = UNDEFINED, int multisynapses_ = NONE, pair<int, int> delayRange_ = {NONE, NONE}, vector<pair<int, int>> sparseConnections_ = {{NONE, NONE}});
    Layer(TopologyParameters &topology, vector<pair<NeuronParameters, int>> &neuronParams, int dt);
    string getType();
    int getNumNeurons();
    string getConnections();
    int getMultisynapses();
    vector<pair<int, int>>& getSparseConnections();
    LIFneuron& getNeuron(int i);
    vector<LIFneuron>& getNeurons();
    void getNeuronsType();

    void setPresynapticLinks(Layer &preLayer);
    void feedForward(int layerId, int time);
};